#include "./headers.h"
#include <stdio.h>

// ======     COSNTANTS         ======

// ====== FUNCTION DECLARATIONS ======

queue *readInputFile();
void clearResources(int);

int main(int argc, char *argv[]) {
  queue *processes;

  signal(SIGINT, clearResources);
  // TODO Initialization
  // 1. Read the input files.
  processes = readInputFile();

  printf(ANSI_GREEN "number of processes: %ld\n" ANSI_RESET, size(processes));

  // 2. Ask the user for the chosen scheduling algorithm and its parameters, if
  // there are any.
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

void clearResources(int signum) {
  // TODO Clears all resources in case of interruption
  destroyClk(true);
  killpg(getpgrp(), SIGINT);
  exit(0);
}
