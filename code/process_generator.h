#pragma once

#include "queue.h"
#include "structs.h"

// ===================================
// ======   PROCESS GENERATOR   ======
// ====== FUNCTION DECLARATIONS ======
// ===================================
//
queue *readInputFile(char *fileName);
void printBanner();
scheduler_type getSchedulerType();
queue *getInput(scheduler_type *, int *);
void createSchedulerAndClock(pid_t *, pid_t *, int, int);
void sendProcessesToScheduler(queue *, int);
