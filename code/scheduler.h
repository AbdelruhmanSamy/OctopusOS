#pragma once

#include "list.h"
#include "structs.h"

scheduler_type getScType(int schedulerType);
void getProcesses(int gen_msgQID, d_list *processTable);
void freeProcessEntry(void *processEntry);
void createProcess(d_list *processTable, process_t *process);
void cleanUpScheduler();
void clearSchResources(int signum);
