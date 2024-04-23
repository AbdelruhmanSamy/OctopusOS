#pragma once

#include "list.h"
#include "minHeap.h"
#include "queue.h"
#include "structs.h"

scheduler_type getScParams(char *argv[], int *quantem);
int getProcess(int *processesFlag, int gen_msgQID, process_t *process);
void freeProcessEntry(void *processEntry);
process_t *createProcess(d_list *processTable, process_t *process);
void cleanUpScheduler();
void clearSchResources(int signum);

//===============================
// Scheduling Algorithms
//===============================
void schedule(scheduler_type schType, int quantem, int gen_msgQID,
              d_list *processTable);
int compareHPF(void *e1, void *e2);
int compareSRTN(void *e1, void *e2);
int HPFScheduling(void *readyQueue, process_t *process, int *rQuantem);
int SRTNScheduling(void *readyQueue, process_t *process, int *rQuantem);
int RRScheduling(void *readyQueue, process_t *process, int *rQuantem);
void contextSwitch();

//===============================
// Preempting Functions
//===============================
// TODO: Handle in preempting functions restoring the process state and update
// stats
// TODO: make sure that we want to update by the index if not make a version
// with pid
void preemptProcessByIndex(d_list *processTable, unsigned int index);
void resumeProcessByIndex(d_list *processTable, unsigned int index);

//===============================
// IPC Functions
//===============================
// TODO: create a queue between scheduler and all processes to be able to signal 
// it's termination
int initSchProQ();
void sigUsr1Handler(int signum);
