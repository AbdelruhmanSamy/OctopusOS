/* ====================================================================
 * scheduler.c
 *
 * scheduler is responsible for creating and managing processes
 * according to a specific algorithm
 * ====================================================================
 */

#include "scheduler.h"
#include "headers.h"
#include <unistd.h>

int msgQID;
d_list *p_table = NULL;

/**
 * main - The main function of the scheduler.
 *
 * @argc: the number of arguments
 * @argv: the arguments
 * return: 0 on success, -1 on failure
 */
int main(int argc, char *argv[]) {
  int key, gen_msgQID, response, quantem;
  scheduler_type schedulerType;
  d_list *processTable = NULL;

  signal(SIGINT, clearSchResources);
  signal(SIGTERM, clearSchResources);
  signal(SIGUSR1, sigUsr1Handler);
  if (atexit(cleanUpScheduler) != 0) {
    perror("atexit");
    exit(1);
  }

  initClk();

  schedulerType = getScParams(argv, &quantem);
  if (DEBUG)
    printf(ANSI_BLUE "==>SCH: My Scheduler Type is %i\n" ANSI_RESET,
           (int)schedulerType);

  p_table = processTable = createList(freeProcessEntry);
  if (!processTable) {
    perror("Error while creating process table");
    exit(-1);
  }

  msgQID = gen_msgQID = initSchGenCom();
  schedule(schedulerType, quantem, gen_msgQID, processTable);

  // TODO Initialize Scheduler
  //  Create Wait queue ??
  //  Create log file
  //
  // TODO Create process when generator tells you it is time
  //  Setup COM between process and Scheduler (init msgs queue)
  //
  // TODO Clear After process termination
  //  Calculate all needed values (till now)
  //  Remove process from processes Table (Delete its PCB)
  //
  // TODO Clean everything when Scheduler finishes or if it is killed
  //  Calculate all needed stats
  //  create pref file
  //  kill all living processes and destroy its PCB (is this right?)
  //  destroy process table
  //  destroy clk
  //  Any other clean up
  return (0);
}

/**
 * Schedule - Main loop of scheduler
 *
 * @schedulerType: scheduler type
 * @quantem: RR quantem
 * @gen_msgQID: msg queue ID between generator & scheduler
 * @processTable: pointer to process table
 */
void schedule(scheduler_type schType, int quantem, int gen_msgQID,
              d_list *processTable) {
  void *readyQ;
  process_t process, *newProcess;
  int processesFlag = 1; // to know when to stop getting processes from gen
  int rQuantem = quantem;
  int currentTime = getClk();
  int lastTime = currentTime;
  int (*algorithm)(void *readyQ, process_t *newProcess, int *rQuantem);

  switch (schType) {
  case HPF:
    readyQ = createMinHeap(compareHPF);
    algorithm = HPFScheduling;
    break;
  case SRTN:
    readyQ = createMinHeap(compareSRTN);
    algorithm = SRTNScheduling;
    break;
  case RR:
    readyQ = createQueue(free);
    algorithm = RRScheduling;
    break;
  default:
    exit(-1);
  }

  while (1) {
    currentTime = getClk();
    if (currentTime == lastTime)
      continue;

    newProcess = NULL;
    if (processesFlag) {
      if (getProcess(&processesFlag, gen_msgQID, &process))
        newProcess = createProcess(processTable, &process);

      algorithm(readyQ, newProcess, &rQuantem);
      if (rQuantem <= 0)
        rQuantem = quantem;
    } else if (empty(readyQ) && processTable->size == 0)
      break;

    lastTime = currentTime;
  }
}

/**
 * compareHPF - compare function for HPF ready queue
 *
 * @e1: pointer to first element
 * @e2: pointer to first element
 * Return: 1 if e1 priority is higher, -1 if e2 is higher, 0 if they are equal
 */
int compareHPF(void *e1, void *e2) {
  if (((process_t *)e1)->priority < ((process_t *)e2)->priority)
    return -1;
  else if (((process_t *)e1)->priority > ((process_t *)e2)->priority)
    return 1;
  return 0;
}

/**
 * compareSRTN - compare function for SRTN ready queue
 *
 * @e1: pointer to first element
 * @e2: pointer to first element
 * Return: 1 if e2 Remaining Time is less, -1 if e1 is less, 0 if they are equal
 */
int compareSRTN(void *e1, void *e2) {
  if (((process_t *)e1)->RT < ((process_t *)e2)->RT)
    return -1;
  else if (((process_t *)e1)->RT > ((process_t *)e2)->RT)
    return 1;
  return 0;
}

/**
 * HPFScheduling - HPF scheduling algorithm
 *
 * @readyQueue: scheduler ready queue
 * @process: pointer to process
 * @rQuantem: remaining quantem time
 *
 * Return: 1 on success, 0 on failure
 */
int HPFScheduling(void *readyQueue, process_t *process, int *rQuantem) {
  min_heap *readyQ = (min_heap *)readyQueue;

  if (process)
    insertMinHeap(&readyQ, process);
  return 1;
}

/**
 * SRTNScheduling - HPF scheduling algorithm
 *
 * @readyQueue: scheduler ready queue
 * @process: pointer to process
 * @rQuantem: remaining quantem time
 *
 * Return: 1 on success, 0 on failure
 */
int SRTNScheduling(void *readyQueue, process_t *process, int *rQuantem) {
  min_heap *readyQ = (min_heap *)readyQueue;
  process_t *currentProcess;

  if (process) {
    insertMinHeap(&readyQ, (void *)process);
    currentProcess = (process_t *)extractMin(readyQ);

    if (DEBUG)
      printf("inside SRTN, RT = %d\n", *(currentProcess->RT));
    fflush(stdout);

    if (process == currentProcess)
      contextSwitch();
  }

  return 1;
}

/**
 * RRScheduling - RR scheduling algorithm
 *
 * @readyQueue: scheduler ready queue
 * @process: pointer to process
 * @rQuantem: remaining quantem time
 *
 * Return: 1 on success, 0 on failure
 */
int RRScheduling(void *readyQueue, process_t *process, int *rQuantem) {
  queue *readyQ = (queue *)readyQueue;

  if (process)
    push(readyQ, process);

  if (!empty(readyQ)) {
    (*rQuantem)--;
    if (*rQuantem <= 0)
      contextSwitch();
  }
  return 1;
}

/**
 * getProcess - gets ap process from generator
 *
 * @gen_msgQID: msg queue with process generator
 * @processTable: process table
 *
 * Return: 0 if no process, 1 if got the process
 */
int getProcess(int *processesFlag, int gen_msgQID, process_t *process) {
  int response;

  response = msgrcv(gen_msgQID, process, sizeof(process_t), 0, IPC_NOWAIT);

  if (response == -1) {
    if (errno == ENOMSG) {
      return 0;
    }
    perror("Error in receiving process from process generator");
    exit(-1);
  }

  if (process->id == -1) {
    printf(ANSI_BLUE "==>SCH: Received All Processes\n" ANSI_RESET);
    *processesFlag = 0;
    return 0;
  }
  printf(ANSI_BLUE "==>SCH: Received process with id = %i\n" ANSI_RESET,
         process->id);
  return 1;
}

/**
 * getScType - gets scheduler type
 *
 * @schedulerType: scheduler type
 * return: scheduler type enum
 */
scheduler_type getScParams(char *argv[], int *quantem) {
  int schedulerType = atoi(argv[1]);

  switch (schedulerType) {
  case 0:
    return HPF;
  case 1:
    return SRTN;
  case 2:
    *quantem = atoi(argv[2]);
    return RR;
  default:
    exit(-1);
  }
}

/**
 * freeProcessEntry - process entry free function
 *
 * @processEntry: process entry
 */
void freeProcessEntry(void *processEntry) {
  if (processEntry)
    free(((process_entry_t *)processEntry)->PCB.process);
  free(processEntry);
}

/**
 * createProcess - create a new process and add it to process table
 *
 * @processTable: pointer to process table
 * @process: pointer to new process info
 */
process_t *createProcess(d_list *processTable, process_t *process) {
  pid_t pid;
  process_entry_t *processEntry;
  process_t *pcbProcess;

  processEntry = malloc(sizeof(*processEntry));
  if (!processEntry) {
    perror("malloc");
    exit(-1);
  }

  pcbProcess = malloc(sizeof(*pcbProcess));
  if (!pcbProcess) {
    perror("malloc");
    exit(-1);
  }

  *pcbProcess = *process;
  processEntry->p_id = pid;
  processEntry->PCB.state = READY;
  processEntry->PCB.process = pcbProcess;

  pid = fork();

  int shmid = initSchProShm(pid);
  int *shmAdd = (int *)shmat(shmid, (void *)0, 0);

  processEntry->PCB.process->RT = shmAdd;
  *processEntry->PCB.process->RT = process->BT;

  if (pid == -1) {
    perror("fork");
    exit(-1);
  }

  if (pid == 0) {
    char *args[] = {"./process.out", NULL};
    execvp(args[0], args);
    kill(getpid(), SIGSTOP);
    exit(0);
  }

  if (!insertNodeEnd(processTable, processEntry)) {
    perror("insertNodeEnd");
    exit(-1);
  }

  printf(ANSI_BLUE "==>SCH: Added process to processes table\n" ANSI_RESET);
  return pcbProcess;
}

void contextSwitch() {
  printf(ANSI_BLUE "==>SCH: Context Switching\n" ANSI_RESET);
  // TODO Context Switch (print to log file in start and finish)
  //  When SRT process comes to ready queue (by myself)
  //  When quantem ends (communicate with clk)
  //  When a process finishes (process get SIGTRM)
  //  When a process gets a signal (SIGKILL, SIGINT, SIGSTP, ...etc)
  //  The Switch
  //    Move old process to ready or wait or (clear after, if it has terminated)
  //    Save PCB if it still exist (set attributes)
  //    Schedule new Process (We need the algo here)
  //    Load PCB (set attributes)
  //    Tell the process to start
}

/**
 * preemptProcessByIndex - Preempt a process by its index in the process table
 * @processTable: pointer to process table
 * @index: index of the process to preempt
 */
void preemptProcessByIndex(d_list *processTable, unsigned int index) {
  process_entry_t *processEntry = getNode(processTable, index)->data;
  PCB_t *pcb = &processEntry->PCB;

  printf(ANSI_GREY "==>SCH: Preempting process with id = %i\n" ANSI_RESET,
         processEntry->p_id);

  if (pcb->state == RUNNING) {
    kill(processEntry->p_id, SIGSTOP);
    pcb->state = READY;
    pcb->process->LST = getClk();
  }
}

/**
 * resumeProcessByIndex - Resume a process by its index in the process table
 * @processTable: pointer to process table
 * @index: index of the process to resume
 */
void resumeProcessByIndex(d_list *processTable, unsigned int index) {
  process_entry_t *processEntry = getNode(processTable, index)->data;
  PCB_t *pcb = &processEntry->PCB;

  printf(ANSI_BLUE "==>SCH: Resuming process with id = %i\n" ANSI_RESET,
         processEntry->p_id);

  if (pcb->state == READY) {
    kill(processEntry->p_id, SIGCONT);
    pcb->state = RUNNING;
    pcb->process->WT += getClk() - pcb->process->LST;
  }
}

/**
 * cleanUpScheduler - Make necessary cleaning
 */
void cleanUpScheduler() {
  msgctl(msgQID, IPC_RMID, (struct msqid_ds *)0);
  destroyClk(true);
  if (p_table)
    destroyList(&p_table);
  killpg(getpgrp(), SIGINT);
}

/**
 * clearSchResources - Clears all resources in case of interruption.
 *
 * @signum: the signal number
 */
void clearSchResources(int signum) {
  cleanUpScheduler();
  exit(0);
}

int initSchProQ() {
  int id = ftok("keyfiles/PRO_SCH_Q", SCH_PRO_COM);
  int q_id = msgget(id, IPC_CREAT | 0666);

  if (q_id == -1) {
    perror("error in creating msg queue between process & scheduler");
    exit(-1);
  } else if (DEBUG) {
    printf("Message queue created sucessfully with pid = %d\n", q_id);
  }

  return q_id;
}

void sigUsr1Handler(int signum) {
  // TODO: write an appropriate implementation for this function
  // detach the scheduler from the sharedmemory of the rem. time
  // of this process (the running one)
  // FIXME: Just tell me why you kill the sch here I spent 1 hour debuging for
  // this
  // raise(SIGKILL);
}
