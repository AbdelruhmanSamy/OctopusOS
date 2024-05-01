// copyright 2024: Abderlrahman Samy - George Magdy - Ahmed Hamed - Amir Anwar
#pragma once

#include <sys/types.h>

typedef enum scheduler_type { HPF, SRTN, RR } scheduler_type;

typedef enum process_state { READY, RUNNING, STOPPED } process_state;

typedef struct process {
  process_state state;
  pid_t pid;
  int id;       // unique identifier
  int AT;       // arrival time
  int BT;       // burst time
  int priority; // priority
  int ST;       // start time
  int FT;       // finish time
  int WT;       // waiting time
  int LST;      // Last Stop time (last time the process was stopped)
                //   used to calculate the waiting time
  int *RT;      // remaining time
  int TA;       // turnaround time
} process_t;

union SemUn {
  int val;               /* Value for SETVAL */
  struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
  unsigned short *array; /* Array for GETALL, SETALL */
  struct seminfo *__buf; /* Buffer for IPC_INFO (Linux-specific) */
};

// To create scheduler.perf.
// The scheduler will create only one instant from this struct
typedef struct perfStats {
  
  // these are incrementally updated each time a process finishes
  int totalWorkingTime;   // sum of burst times
  int totalWaitingTime;   // sum of waiting times
  double totalWTA;        // sum of weighted turnaround times
  int numFinished;        // number of finished processes
  
  // and these are calculated when the last process finishes
  double CPU_utilization; // = totalWorkingTime / totalTime
  double avgWaitingTime;  // totalWaitingTime / N
  double avgWTA;          // totalWTA / N
  double stdWTA;          // Standard deviation of weighted turnaround time
                          //= SUM( (WTA[i] - avgWTA)^2 ) / N
} perfStats;