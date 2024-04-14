/* ====================================================================
 * process_generator.c
 *
 * The process generator is responsible for reading the input file and
 * sending the processes to the scheduler at the appropriate time.
 * assigned to: amir-kedis
 * ====================================================================
 */

#include "process_generator.h"
#include "headers.h"

/**
 * main - The main function of the process generator.
 *
 * @argc: the number of arguments
 * @argv: the arguments
 * return: 0 on success, -1 on failure
 */
int main(int argc, char *argv[]) {
  queue *processes;
  scheduler_type schedulerType;
  int quantum;
  pid_t schedulerPid, clockPid;
  int msgQID;

  signal(SIGINT, clearResources);
  signal(SIGTERM, clearResources);
  if (atexit(cleanUp) != 0) {
    perror("atexit");
    exit(1);
  }

  printBanner();

  processes = readInputFile();
  printf(ANSI_GREEN "number of processes: %ld\n" ANSI_RESET, size(processes));

  getInput(&schedulerType, &quantum);

  createSchedulerAndClock(&schedulerPid, &clockPid, (int)schedulerType);

  initClk();
  msgQID = initSchGenCom();

  sendProcessesToScheduler(processes, msgQID);
  destroyQueue(processes);

  if (wait(NULL) == -1) {
    perror("wait");
    exit(1);
  }
  destroyClk(true);
}

/**
 * readInputFile - Reads the input file and stores the data in the data
 * structure.
 *
 * return: a pointer to the queue of processes
 */
queue *readInputFile() {
  FILE *file;
  char *line = NULL;
  size_t len = 0;
  queue *processes = createQueue(free);

  printf(ANSI_YELLOW "============================" ANSI_RESET "\n");
  printf(ANSI_YELLOW "|| Reading processes file ||" ANSI_RESET "\n");
  printf(ANSI_YELLOW "============================" ANSI_RESET "\n");

  file = fopen("processes.txt", "r");
  if (file == NULL) {
    perror("Error in opening file");
    exit(-1);
  }

  while (getline(&line, &len, file) != -1) {

    if (line[0] == '#') {
      continue;
    }
    process_t *newProcess = malloc(sizeof(process_t));
    sscanf(line, "%d %d %d %d", &newProcess->id, &newProcess->AT,
           &newProcess->BT, &newProcess->priority);
    push(processes, newProcess);
    printf(ANSI_PURPLE "=>Inserted process with id: %d, AT: %d, BT: %d, "
                       "priority: %d into processes queue\n" ANSI_RESET,
           newProcess->id, newProcess->AT, newProcess->BT,
           newProcess->priority);
  }

  free(line);
  fclose(file);

  printf(ANSI_YELLOW "============================" ANSI_RESET "\n");
  printf(ANSI_YELLOW "|| Finished reading file   ||" ANSI_RESET "\n");
  printf(ANSI_YELLOW "============================" ANSI_RESET "\n");

  return processes;
}

/**
 * getSchedulerType - Asks the user to choose the scheduling algorithm to start
 * the simulation.
 *
 * return: the chosen scheduler type
 */
scheduler_type getSchedulerType() {
  int choice;

  printf(ANSI_TEAL "==========================================================="
                   "=\n" ANSI_RESET);
  printf(
      ANSI_TEAL
      "|| Choose the scheduling algorithm to start the simulation ||" ANSI_RESET
      "\n");
  printf(ANSI_TEAL "==========================================================="
                   "=\n" ANSI_RESET);

  printf(ANSI_GREEN
         "1. Non-preemptive Highest Priority First (HPF)\n" ANSI_RESET);
  printf(ANSI_GREEN "2. Shortest Remaining Time Next (SRTN)\n" ANSI_RESET);
  printf(ANSI_GREEN "3. Round Robin (RR)\n" ANSI_RESET);
  scanf("%d", &choice);

  switch (choice) {
  case 1:
    return HPF;
  case 2:
    return SRTN;
  case 3:
    return RR;
  default:
    exit(-1);
  }
}

/**
 * getInput - Asks the user for the chosen scheduling algorithm and its
 * parameters, if there are any.
 *
 * @schedulerType: a pointer to the chosen scheduler type
 * @quantum: a pointer to the quantum value
 */
void getInput(scheduler_type *schedulerType, int *quantum) {
  *schedulerType = getSchedulerType();

  if (*schedulerType == -1) {
    printf(ANSI_RED "Invalid choice, try again...\n" ANSI_RESET);
    return getInput(schedulerType, quantum);
  }

  if (*schedulerType == RR) {
    printf(ANSI_YELLOW "Enter the quantum value: " ANSI_RESET);
    scanf("%d", quantum);

    if (*quantum <= 0) {
      printf(ANSI_RED "Invalid quantum value, try again...\n" ANSI_RESET);
      return getInput(schedulerType, quantum);
    }
  }

  printf(ANSI_GREEN "============================\n" ANSI_RESET);
  printf(ANSI_GREEN "||       PARAMETERS       ||\n" ANSI_RESET);
  printf(ANSI_GREEN "============================\n" ANSI_RESET);
  printf(ANSI_GREEN "|| Scheduler type |  %d    ||\n" ANSI_RESET,
         *schedulerType);
  if (*schedulerType == RR) {
    printf(ANSI_GREEN "|| Quantum:       |  %d    ||\n" ANSI_RESET, *quantum);
  }
  printf(ANSI_GREEN "============================\n" ANSI_RESET);
}

/**
 * printBanner - Prints the banner of the process generator.
 */
void printBanner() {
  printf(ANSI_PURPLE);
  printf("                              ___\n"
         "                           .-'   `'.\n"
         "                          /         \\\n"
         "                          |         ;\n"
         "                          |         |           ___.--,\n"
         "                 _.._     |0) ~ (0) |    _.---'`__.-( (_.\n"
         "          __.--'`_.. '.__.\\    '--. \\_.-' ,.--'`     \\\"\\\"\"\n"
         "         ( ,.--'`   ',__ /./;   ;, '.__.'`    __\n"
         "         _`) )  .---.__.' / |   |\\   \\__..--\"\"\"\"  \"\"--.,_\n"
         "        `---' .'.''-._.-'`_./  /\\ '.  \\ _.-~~~````~~~-._`-.__.'\n"
         "              | |  .' _.-' |  |  \\  \\  '.               `~---`\n"
         "               \\ \\/ .'     \\  \\   '. '-._)\n"
         "                \\/ /        \\  \\    `=.__`~-.\n"
         "                / /\\         `) )    / / \\\"\\\"\".`\\\n"
         "          , _.-'.'\\ \\        / /    ( (     / /\n"
         "           `--~`   ) )    .-'.'      '.'.  | (\n"
         "                  (/`    ( (`          ) )  '-;\n"
         "                   `      '-;         (-'\n");

  printf(
      "________          __                             ________    "
      "_________ \n"
      "\\_____  \\   _____/  |_  ____ ______  __ __  _____\\_____  \\  /   "
      "_____/ \n"
      " /   |   \\_/ ___\\   __\\/  _ \\____ \\|  |  \\/  ___//   |   \\ "
      "\\_____ "
      " \\  \n"
      "/    |    \\  \\___|  | (  <_> )  |_> >  |  /\\___ \\/    |    \\/      "
      "  \\ \n"
      "\\_______  /\\___  >__|  \\____/|   __/|____//____  >_______  /_______ "
      " / \n"
      "        \\/     \\/            |__|              \\/        \\/        "
      "\\/  \n");

  sleep(1);
  printf("Welcome to OctopusOS\n");
  sleep(1);
  printf(ANSI_RESET);
}

/**
 * createSchedulerAndClock - Creates the scheduler and clock processes.
 *
 * @schedulerPid: a pointer to the scheduler process id
 * @clockPid: a pointer to the clock process id
 */
void createSchedulerAndClock(pid_t *schedulerPid, pid_t *clockPid,
                             int schedulerType) {

  *clockPid = fork();

  if (*clockPid == -1) {
    perror("Error in creating clock process");
    exit(-1);
  }

  if (*clockPid == 0) {
    char *args[] = {"./clk.out", NULL};
    execvp(args[0], args);
    exit(0);
  }

  *schedulerPid = fork();

  if (*schedulerPid == -1) {
    perror("Error in creating scheduler process");
    exit(-1);
  }

  if (*schedulerPid == 0) {
    char *type;

    switch (schedulerType) {
    case 0:
      type = "0";
      break;
    case 1:
      type = "1";
      break;
    case 2:
      type = "2";
      break;
    default:
      exit(-1);
    }

    char *args[] = {"./scheduler.out", type, NULL};
    execvp(args[0], args);
    exit(0);
  }

  printf(ANSI_PURPLE "|| ==>   Created Scheduler and Clock\n" ANSI_RESET);
  printf(ANSI_PURPLE "||    ==> Scheduler PID: %d\n" ANSI_RESET, *schedulerPid);
  printf(ANSI_PURPLE "||    ==> Clock     PID: %d\n" ANSI_RESET, *clockPid);
}

/**
 * sendProcessesToScheduler - Sends the processes to the scheduler at the
 * appropriate time.
 *
 * @processes: a pointer to the queue of processes
 */
void sendProcessesToScheduler(queue *processes, int msgQID) {
  int currentTime = getClk();
  int lastTime = currentTime;
  process_t *process;
  int response;

  while (!empty(processes)) {
    process = (process_t *)front(processes);
    currentTime = getClk();
    if (currentTime == lastTime) {
      lastTime = currentTime;
      continue;
    }

    // TODO: printing current time should be the responsibility of the scheudler
    // or clock
    printf(ANSI_YELLOW "=>Current time: %d\n" ANSI_RESET, currentTime);
    if (currentTime < process->AT) {
      lastTime = currentTime;
      continue;
    }

    printf(ANSI_PURPLE "=>Sending process with id: %d, AT: %d, BT: %d, "
                       "priority: %d to scheduler\n" ANSI_RESET,
           process->id, process->AT, process->BT, process->priority);

    response = msgsnd(msgQID, process, sizeof(process_t), !IPC_NOWAIT);
    if (response == -1) {
      perror("Error in sending process to scheduler\n");
      exit(-1);
    }

    pop(processes);
    lastTime = currentTime;
  }

  process->id = -1;
  response = msgsnd(msgQID, process, sizeof(process_t), !IPC_NOWAIT);
  if (response == -1) {
    perror("Error in terminating sending processes to scheduler\n");
    exit(-1);
  }
}
