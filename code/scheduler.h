#pragma once

#include "list.h"
#include "structs.h"

scheduler_type getScType(int schedulerType);
void getProcesses(int gen_msgQID, d_list *processTable);
void freeProcessEntry(void *processEntry);
void createProcess(d_list *processTable, process_t *process);
void cleanUpScheduler();
void clearSchResources(int signum);

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

