#pragma once

#include "list.h"
#include "structs.h"

scheduler_type getScParams(char *argv[], int *quantem);
int getProcess(int *processesFlag, int gen_msgQID, process_t *process);
void freeProcessEntry(void *processEntry);
void createProcess(d_list *processTable, process_t *process);
void cleanUpScheduler();
void clearSchResources(int signum);

//===============================
// Scheduling Algorithms
//===============================
int RRScheduling(int quantem, int gen_msgQID, d_list *processTable);

//===============================
// Preempting Functions
//===============================
// TODO: Handle in preempting functions restoring the process state and update
// stats
// TODO: make sure that we want to update by the index if not make a version
// with pid
void preemptProcessByIndex(d_list *processTable, unsigned int index);
void resumeProcessByIndex(d_list *processTable, unsigned int index);
// TODO: make 2 functions for start and termination
