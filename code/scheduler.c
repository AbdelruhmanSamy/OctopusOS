/* ====================================================================
 * scheduler.c
 *
 * scheduler is responsible for creating and managing processes
 * according to a specific algorithm
 * ====================================================================
 */

#include "scheduler.h"
#include "headers.h"
#include "minHeap.h"
#include "queue.h"
#include "structs.h"
#include <assert.h>
#include <unistd.h>

process_t *currentProcess = NULL;
perfStats stats;
int semid;
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

  signal(SIGINT, clearSchResources);
  signal(SIGTERM, clearSchResources);
  signal(SIGUSR1, sigUsr1Handler);

  initClk();

  schedulerType = getScParams(argv, &quantem);
  if (DEBUG)
    printf(ANSI_BLUE "==>SCH: My Scheduler Type is %i\n" ANSI_RESET,
           (int)schedulerType);

  createLogFile();
  gen_msgQID = initSchGenCom();

  schedule(schedulerType, quantem, gen_msgQID);

  return (0);
}

void initPerformanceStats() {
  stats.numFinished = 0;
  stats.totalWaitingTime = 0;
  stats.totalWorkingTime = 0;
  stats.totalWTA = 0;
}

/**
 * Schedule - Main loop of scheduler
 *
 * @schedulerType: scheduler type
 * @quantem: RR quantem
 * @gen_msgQID: msg queue ID between generator & scheduler
 * @processTable: pointer to process table
 */
void schedule(scheduler_type schType, int quantem, int gen_msgQID) {
  void *readyQ;
  process_t process, *newProcess;
  int processesFlag = 1; // to know when to stop getting processes from gen
  int rQuantem = quantem;
  int quantemClk = 0, currentClk = 0;
  int (*algorithm)(void **readyQ, process_t *newProcess, int *rQuantem);

  semid = initSchProSem();   //initializing semaphor to control RT shared mem.
  SemUn semun;
  semun.val = 1;
  if (semctl(semid, 0, SETVAL, semun) == -1){   
        perror("Error in semctl");
        exit(-1);
  }

  initPerformanceStats();
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
    readyQ = createQueue(freeQueueData);
    algorithm = RRScheduling;
    break;
  default:
    exit(-1);
  }

  quantemClk = getClk();
  int lastClk = quantemClk;
  while (1) {
    currentClk = getClk();

    if (currentClk - quantemClk >= quantem) {
      quantemClk = currentClk;
      rQuantem = 0;
    }

    if (currentClk != lastClk) {
      printf(ANSI_GREY "========================================\n" ANSI_RESET);
      printf(ANSI_BLUE "==>SCH: Current Clk = %i\n" ANSI_RESET, currentClk);

      if (currentProcess) {
        int remTime = getRemTime(currentProcess);
        if(remTime > 0){
          printf(ANSI_BLUE "==>SCH:" ANSI_GREEN " Process %d " ANSI_BOLD
                          "RT = %i\n" ANSI_RESET,
                currentProcess->id, remTime);
        }
      }
    }

    newProcess = NULL;
    if (processesFlag) {
      while (getProcess(&processesFlag, gen_msgQID, &process)) {
        newProcess = createProcess(&process);
        algorithm(&readyQ, newProcess, &rQuantem);
      }
    }
    newProcess = NULL;

    if (!algorithm(&readyQ, newProcess, &rQuantem) && !processesFlag &&
        !currentProcess)
      break;

    if (rQuantem <= 0) {
      quantemClk = currentClk;
      rQuantem = quantem;
    }

    lastClk = currentClk;
  }

  switch (schType) {
  case HPF:
  case SRTN:
    destroyHeap(readyQ);
    break;
  case RR:
    destroyQueue(readyQ);
    break;
  default:
    exit(-1);
  }

  writePerfFile();
  printf(ANSI_BLUE "==>SCH: " ANSI_RED ANSI_BOLD
                   "Scheduler Finished\n" ANSI_RESET);

  cleanUpScheduler();
}

void freeQueueData(void *data) { return; }

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
  if(getRemTime((process_t *)e1) < getRemTime((process_t *)e2))
    return -1;
  if(getRemTime((process_t *)e1) > getRemTime((process_t *)e2))
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
 * Return: 0 if no process is no the system, 1 otherwise
 */
int HPFScheduling(void **readyQueue, process_t *process, int *rQuantem) {
  min_heap **readyQ = (min_heap **)readyQueue;
  process_t *newScheduledProcess = NULL;
  (void)rQuantem;

  if (process) {
    insertMinHeap(readyQ, process);
    return 1;
  }

  if (!currentProcess && !getMin(*readyQ))
    return 0;

  if (!currentProcess) {
    newScheduledProcess = (process_t *)extractMin(*readyQ);
    contextSwitch(newScheduledProcess);
  }
  return 1;
}

/**
 * SRTNScheduling - HPF scheduling algorithm
 *
 * @readyQueue: scheduler ready queue
 * @process: pointer to process
 * @rQuantem: remaining quantem time
 *
 * Return: 0 if no process is no the system, 1 otherwise
 */
int SRTNScheduling(void **readyQueue, process_t *process, int *rQuantem) {
  min_heap **readyQ = (min_heap **)readyQueue;
  process_t *newScheduledProcess = NULL;
  (void)rQuantem;

  if (process) {
    insertMinHeap(readyQ, process);
    return 1;
  }

  if (!currentProcess && !getMin(*readyQ))
    return 0;

  if (!currentProcess) {
    newScheduledProcess = (process_t *)extractMin(*readyQ);
    contextSwitch(newScheduledProcess);
  } else if (getMin(*readyQ) &&
             compareSRTN(getMin(*readyQ), currentProcess) < 0) {
    preemptProcess(currentProcess);
    newScheduledProcess = (process_t *)extractMin(*readyQ);
    insertMinHeap(readyQ, currentProcess);
    currentProcess = NULL;
    contextSwitch(newScheduledProcess);
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
 * Return: 0 if no process is no the system, 1 otherwise
 */
int RRScheduling(void **readyQueue, process_t *process, int *rQuantem) {
  queue **readyQ = (queue **)readyQueue;
  process_t *newScheduledProcess = NULL;

  if (process) {
    push(*readyQ, process);
    return 1;
  }

  if (!(currentProcess) && empty(*readyQ))
    return 0;

  if (!currentProcess) {
    *rQuantem = -1;
    contextSwitch((process_t *)pop(*readyQ));
  } else if (*rQuantem <= 0) {
    preemptProcess(currentProcess);
    push(*readyQ, currentProcess);
    currentProcess = NULL;
    contextSwitch((process_t *)pop(*readyQ));
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
    printf(ANSI_BLUE "==>SCH: " ANSI_RED ANSI_BOLD
                     "Received All Processes\n" ANSI_RESET);
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
 * createProcess - create a new process and add it to process table
 *
 * @processTable: pointer to process table
 * @process: pointer to new process info
 */
process_t *createProcess(process_t *process) {
  pid_t pid;
  process_t *newProcess;

  newProcess = malloc(sizeof(*newProcess));
  if (!newProcess) {
    perror("malloc");
    exit(-1);
  }

  *newProcess = *process;
  newProcess->state = READY;

  pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(-1);
  }

  if (pid == 0) {
    char *args[] = {"./process.out", NULL};
    // NOTE: If you want to autostart the process, uncomment the next line
    execvp(args[0], args);
    exit(0);
  }

  newProcess->pid = pid;
  kill(pid, SIGSTOP);
  // printf(ANSI_BLUE "==>SCH: Created process with id = %i\n" ANSI_RESET,
  // newProcess->pid);

  //initilizing RT shared mem.
  int shmid = initSchProShm(pid);
  int *shmAdd = (int *)shmat(shmid, (void *)0, 0);

  
  newProcess->RT = shmAdd;

  setRemTime(newProcess , process->BT);
  
  return newProcess;
}

void contextSwitch(process_t *newProcess) {
  if (newProcess) {
    currentProcess = newProcess;

    if (newProcess->state == READY) {
      startProcess(newProcess);
    } else if (newProcess->state == STOPPED) {
      resumeProcess(newProcess);
    }
  }
}

/**
 * startProcess - Start a process by pid
 * @process: pointer to process
 */
void startProcess(process_t *process) {
  if (process) {
    printf(ANSI_BLUE "==>SCH: Starting process with id = %i\n" ANSI_RESET,
           process->pid);

    process->state = RUNNING;

    process->WT = getClk() - process->AT;

    // log this
    logger("started", process);
    kill(process->pid, SIGCONT);
  }
}

/**
 * preemptProcessByIndex - Preempt a process by its index in the process
 * table
 * @process: pointer to process
 */
void preemptProcess(process_t *process) {
  if (process) {
    printf(ANSI_GREY "==>SCH: Preempting process with id = %i\n" ANSI_RESET,
           process->pid);

    if (process->state == RUNNING) {
      kill(process->pid, SIGSTOP);
      process->state = STOPPED;

      // log this
      logger("stopped", process);
    }
  }
}

/**
 * resumeProcessByIndex - Resume a process by its index in the process table
 * @process: pointer to process
 */
void resumeProcess(process_t *process) {
  if (process) {

    printf(ANSI_BLUE "==>SCH: Resuming process with id = %i\n" ANSI_RESET,
           process->pid);

    if (process->state == STOPPED) {
      kill(process->pid, SIGCONT);
      process->state = RUNNING;

      // log this
      logger("resumed", process);
    }
  }
}

/**
 * cleanUpScheduler - Make necessary cleaning
 */
void cleanUpScheduler() {
  msgctl(initSchGenCom(), IPC_RMID, (struct msqid_ds *)0);
  
    //deleting semaphor 
  semctl(semid , 1 , IPC_RMID);
  destroyClk(true);
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

void sigUsr1Handler(int signum) {
  pid_t killedProcess;
  int status;
  killedProcess = wait(&status);
  
  currentProcess->TA = getClk() - currentProcess->AT;
  logger("finished", currentProcess);

  if(currentProcess)    //FIXME: added to avoid double freeing currentProcess pointer (however it's expected to be unfreed *isn't this true?*)
    free(currentProcess);
  currentProcess = NULL;
}


void createLogFile() {
  FILE *logFileptr = fopen(LOG_FILE, "w");
  if (logFileptr == NULL) {
    perror("Can't Create Log File");
    exit(-1);
  }

  printf("Started Logging\n");
  fclose(logFileptr);
}

void logger(char *msg, process_t *p) {

  // appending to the previously created file
  FILE *logFileptr = fopen(LOG_FILE, "a");

  if (logFileptr == NULL) {
    perror("Can't Open Log File");
    exit(-1);
  }
  int clk = getClk();
  float WTA = p->TA / (float)p->BT;


  fprintf(logFileptr,
          "At time %i process %i %s arr %i total %i remain %i wait %i", clk,
          p->id, msg, p->AT, p->BT, *p->RT, p->WT);

  if (strcmp(msg, "finished") == 0) {
    fprintf(logFileptr, " TA %i WTA %.2f", p->TA, WTA);
    stats.WTAs[stats.numFinished] = WTA;
    stats.numFinished += 1;
    stats.totalWorkingTime += p->BT;
    stats.totalWaitingTime += p->WT;
    stats.totalWTA += WTA;
  }

  fprintf(logFileptr, "\n");

  fclose(logFileptr);
}

void writePerfFile() {
  FILE *perfFile = fopen(PERF_FILE, "w");

  if (perfFile == NULL) {
    perror("Can't open perf file");
    exit(-1);
  }

  int finalTime = getClk();

  stats.CPU_utilization = 100.0 * stats.totalWorkingTime / finalTime;
  stats.avgWTA = (double)stats.totalWTA / stats.numFinished;
  stats.avgWaitingTime = (double)stats.totalWaitingTime / stats.numFinished;

  // calculate STD Deviation of Weighted Turn around time
  double sumSquaresErr = 0;
  for (int i = 0; i < stats.numFinished; i++) {
    sumSquaresErr +=
        (stats.WTAs[i] - stats.avgWTA) * (stats.WTAs[i] - stats.avgWTA);
  }

  stats.stdWTA = sumSquaresErr / stats.numFinished;

  fprintf(perfFile, "CPU utilization = %.2f%%\n", stats.CPU_utilization);
  fprintf(perfFile, "Avg WTA = %.2f\n", stats.avgWTA);
  fprintf(perfFile, "Avg Waiting = %.2f\n", stats.avgWaitingTime);
  fprintf(perfFile, "Std WTA = %.2f\n", stats.stdWTA);
}

int getRemTime(process_t* p){  
  down(semid);
  int val = *(p->RT);
  up(semid);

  return val;
}

void setRemTime(process_t* p ,int val){
  down(semid);
    *(p->RT) = val;
  up(semid);
}