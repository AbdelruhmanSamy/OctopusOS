#include "headers.h"
#include "list.h"

int msgQID;
d_list *p_table = NULL;

void cleanUpScheduler() {
  msgctl(msgQID, IPC_RMID, (struct msqid_ds *)0);
  destroyClk(true);
  if (p_table)
    destroyList(&p_table);
  killpg(getpgrp(), SIGINT);
}

/**
 * clearResources - Clears all resources in case of interruption.
 *
 * @signum: the signal number
 */
void clearSchResources(int signum) {
  cleanUpScheduler();
  exit(0);
}

scheduler_type getScType(int schedulerType) {
  switch (schedulerType) {
  case 0:
    return HPF;
  case 1:
    return SRTN;
  case 2:
    return RR;
  default:
    exit(-1);
  }
}

void freeProcessEntry(void *processEntry) {
  if (processEntry)
    free(((process_entry_t *)processEntry)->PCB);
  free(processEntry);
}

void createProcess(d_list *processTable, process_t *process) {
  pid_t pid;
  process_entry_t *processEntry;
  PCB_t *pcb;

  pid = fork();

  if (pid == -1) {
    perror("fork");
    exit(-1);
  }

  if (pid == 0) {
    char *args[] = {"./process.out", NULL};
    execvp(args[0], args);
    exit(0);
  }

  processEntry = malloc(sizeof(*processEntry));
  if (!processEntry) {
    perror("malloc");
    exit(-1);
  }

  pcb = malloc(sizeof(*pcb));
  if (!pcb) {
    perror("malloc");
    exit(-1);
  }

  pcb->state = READY;
  pcb->process = *process;
  processEntry->p_id = pid;
  processEntry->PCB = pcb;
  if (!insertNodeEnd(processTable, processEntry)) {
    perror("insertNodeEnd");
    exit(-1);
  }

  printf(ANSI_GREEN "==>SCH: Added process to processes table\n" ANSI_RESET);
}

int main(int argc, char *argv[]) {
  int key, gen_msgQID, response;
  scheduler_type schedulerType;
  process_t tmpProcess;
  d_list *processTable = NULL;

  signal(SIGINT, clearSchResources);
  signal(SIGTERM, clearSchResources);
  if (atexit(cleanUpScheduler) != 0) {
    perror("atexit");
    exit(1);
  }

  initClk();

  schedulerType = getScType(atoi(argv[1]));
  printf(ANSI_YELLOW "==>SCH: My Scheduler Type is %i\n" ANSI_RESET,
         (int)schedulerType);

  p_table = processTable = createList(freeProcessEntry);
  if (!processTable) {
    perror("Error while creating process table");
    exit(-1);
  }

  key = ftok("keyfiles/SCH_GEN_COM", SCH_GEN_COM);
  msgQID = gen_msgQID =
      msgget(key, 0666 | IPC_CREAT); // COM with process_generator

  if (gen_msgQID == -1) {
    perror("Error in creating message queue");
    exit(-1);
  }

  while (1) {
    response =
        msgrcv(gen_msgQID, &tmpProcess, sizeof(process_t), 0, !IPC_NOWAIT);

    if (response == -1) {
      perror("Error in receiving process from process generator");
      exit(-1);
    }

    if (tmpProcess.id == -1) {
      printf(ANSI_YELLOW "==>SCH: Received All Processes\n" ANSI_RESET);
      break;
    }
    printf(ANSI_BLUE "==>SCH: Received process with id = %i\n" ANSI_RESET,
           tmpProcess.id);
    createProcess(processTable, &tmpProcess);
  }

  msgctl(msgQID, IPC_RMID, (struct msqid_ds *)0);

  // TODO Initialize Scheduler
  //  Create Ready queue & (Wait queue ??)
  //  Create log file
  //
  // TODO Create process when generator tells you it is time
  //  Setup COM between process and Scheduler (init msgs queue)
  //
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

  destroyClk(true);
}
