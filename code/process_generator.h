#pragma once

#include "queue.h"
#include "structs.h"

// ===================================
// ======   PROCESS GENERATOR   ======
// ====== FUNCTION DECLARATIONS ======
// ===================================
//
queue *readInputFile();
void printBanner();
scheduler_type getSchedulerType();
void getInput(scheduler_type *, int *);
void createSchedulerAndClock(pid_t *, pid_t *, int);
void sendProcessesToScheduler(queue *, int);
