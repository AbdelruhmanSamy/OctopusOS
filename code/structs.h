// copyright 2024: Abderlrahman Samy - George Magdy - Ahmed Hamed - Amir Anwar
#pragma once

typedef enum scheduler_type { HPF, SRTN, RR } scheduler_type;

typedef enum process_state { READY, RUNNING, BLOCKED } process_state;

typedef struct process {
  int id;       // unique identifier
  int AT;       // arrival time
  int BT;       // burst time
  int priority; // priority
  // TODO: add more attributes here when needed
} process_t;

typedef struct PCB {
  process_state state;
  process_t process;
} PCB_t;

typedef struct process_entry {
  int p_id;
  PCB_t *PCB;
} process_entry_t;

typedef struct cleanup_data {
  int msgQID;
  void *processTable;
} cleanup_data_t;
