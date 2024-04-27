#pragma once

#include "list.h"
#include "minHeap.h"
#include "queue.h"
#include "structs.h"

scheduler_type getScParams(char *argv[], int *quantem);
int getProcess(int *processesFlag, int gen_msgQID, process_t *process);
void freeProcessEntry(void *processEntry);
process_t *createProcess(process_t *process);
void cleanUpScheduler();
void clearSchResources(int signum);

//===============================
// Scheduling Algorithms
//===============================
void schedule(scheduler_type schType, int quantem, int gen_msgQID);
void freeQueueData(void *data);
int compareHPF(void *e1, void *e2);
int compareSRTN(void *e1, void *e2);
int HPFScheduling(void **readyQueue, process_t *process, int *rQuantem);
int SRTNScheduling(void **readyQueue, process_t *process, int *rQuantem);
int RRScheduling(void **readyQueue, process_t *process, int *rQuantem);
void contextSwitch(process_t *newProcess);

//===============================
// Preempting Functions
//===============================
// TODO: Handle in preempting functions restoring the process state and update
// stats
// TODO: make sure that we want to update by the index if not make a version
// with pid
void startProcess(process_t *process);
void preemptProcess(process_t *process);
void resumeProcess(process_t *process);

//===============================
// IPC Functions
//===============================
// TODO: create a queue between scheduler and all processes to be able to signal
// it's termination
int initSchProQ();
void sigUsr1Handler(int signum);

//==============================
// Logging Functions
//==============================

void createLogFile();
void logger(char * action, process_t* process_pcb);