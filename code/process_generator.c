#include "./headers.h"
#include <stdio.h>

// ======     COSNTANTS         ======

// ====== FUNCTION DECLARATIONS ======

queue *readInputFile();
void printBanner();
enum scheduler_type getSchedulerType();
void getInput(enum scheduler_type *, int *);
void clearResources(int);

int main(int argc, char *argv[]) {
  queue *processes;
  enum scheduler_type schedulerType;
  int quantum;

  signal(SIGINT, clearResources);

  printBanner();
  processes = readInputFile();
  printf(ANSI_GREEN "number of processes: %ld\n" ANSI_RESET, size(processes));

  // 2. Ask the user for the chosen scheduling algorithm and its parameters, if
  // there are any.
  getInput(&schedulerType, &quantum);
  // 3. Initiate and create the scheduler and clock processes.
  // 4. Use this function after creating the clock process to initialize clock
  initClk();
  // To get time use this
  int x = getClk();
  printf("current time is %d\n", x);
  // TODO Generation Main Loop
  // 5. Create a data structure for processes and provide it with its
  // parameters.
  // 6. Send the information to the scheduler at the appropriate time.
  // 7. Clear clock resources
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
  queue *processes = createQueue();

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
enum scheduler_type getSchedulerType() {
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
    return -1;
  }
}

/**
 * getInput - Asks the user for the chosen scheduling algorithm and its
 * parameters, if there are any.
 *
 * @schedulerType: a pointer to the chosen scheduler type
 * @quantum: a pointer to the quantum value
 */
void getInput(enum scheduler_type *schedulerType, int *quantum) {
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
         "           jgs  / /\\         `) )    / / \\\"\\\"\".`\\\n"
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

  printf(ANSI_RESET);
}

/**
 * clearResources - Clears all resources in case of interruption.
 *
 * @signum: the signal number
 */
void clearResources(int signum) {
  // TODO: Clears all resources in case of interruption
  destroyClk(true);
  killpg(getpgrp(), SIGINT);
  exit(0);
}
