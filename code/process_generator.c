#include "./headers.h"
#include <stdio.h>
#define MAX_PROCESSES 1000
void clearResources(int);
void readProcesses(process_t ***processes);

int main(int argc, char *argv[]) {
  process_t **processes = malloc(MAX_PROCESSES * sizeof(process_t *));
  for (int i = 0; i < MAX_PROCESSES; i++) {
    processes[i] = NULL;
  }

  signal(SIGINT, clearResources);
  // TODO Initialization
  // 1. Read the input files.
  readProcesses(&processes);

  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (processes[i] == NULL) {
      break;
    }
    printf("Process %d: AT: %d, BT: %d, Priority: %d\n", processes[i]->id,
           processes[i]->AT, processes[i]->BT, processes[i]->priority);
  }

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

void readProcesses(process_t ***processes) {
  process_t *newProcess;
  FILE *file;
  int i = 0;

  file = fopen("processes.txt", "r");

  if (file == NULL) {
    perror("Error in opening file");
    return;
  }
  fscanf(file, "%*[^\n]\n"); // skip the first line
  // while (!feof(file)) {
  //   int ID, AT, BT, priority;
  //   fscanf(file, "%d %d %d %d", &ID, &AT, &BT, &priority);
  //   printf("ID: %d, AT: %d, BT: %d, Priority: %d\n", ID, AT, BT, priority);
  // }

  while (!feof(file)) {
    newProcess = malloc(sizeof(process_t));
    fscanf(file, "%d %d %d %d", &newProcess->id, &newProcess->AT,
           &newProcess->BT, &newProcess->priority);
    (*processes)[i] = newProcess;
    i++;
  }
}

void clearResources(int signum) {
  // TODO Clears all resources in case of interruption
  destroyClk(true);
  killpg(getpgrp(), SIGINT);
  exit(0);
}
