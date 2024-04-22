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

//===============================
// IPC Functions
//===============================
// TODO: create a queue between scheduler and all processes to be able to signal 
// it's termination
int initSchProQ();
void sigUsr1Handler(int signum);
