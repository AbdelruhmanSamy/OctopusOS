#pragma once

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h> //if you don't use scanf/printf change this include
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//===============================
// CONSTANTS
//===============================

// Colors
#define ANSI_GREEN "\x1b[32m"
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_GREY "\x1b[90m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_TEAL "\x1b[96m"
#define ANSI_WHITE "\x1b[37m"
#define ANSI_PURPLE "\x1b[35m"
#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"

#define true 1
#define false 0

#define SCH_GEN_COM 5
#define SCH_PRO_COM 6

#define SHKEY 300

#define LOG_FILE "scheduler.log"
#define MEMORY_FILE "memory.log"
#define PERF_FILE "scheduler.perf"

#define TOTAL_MEMORY_SIZE 1024
#define MAX_PROCESS_SIZE 256

//===============================
// ARGUMENTS
//===============================
const int DEBUG = false; // set to true to enable debug prints
const int DELAY = false; // set to true to add delay

///==============================
// don't mess with this variable//
int *shmaddr;
//===============================

int getClk() { return *shmaddr; }

/*
 * All process call this function at the beginning to establish communication
 * between them and the clock module. Again, remember that the clock is only
 * emulation!
 */
void initClk() {
  int shmid = shmget(SHKEY, 4, 0444);
  while ((int)shmid == -1) {
    // Make sure that the clock exists
    if (DEBUG)
      printf("Wait! The clock not initialized yet!\n");
    sleep(1);
    shmid = shmget(SHKEY, 4, 0444);
  }
  shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of
 * simulation. It terminates the whole system and releases resources.
 */
void destroyClk(bool terminateAll) {
  shmdt(shmaddr);
  if (terminateAll) {
    killpg(getpgrp(), SIGINT);
  }
}

//===============================
// CUSTOM COMMON FUNCTIONS
//===============================

/**
 * cleanUp - Make necessary cleaning
 */
void cleanUp() {
  destroyClk(true);
  killpg(getpgrp(), SIGINT);
}

/**
 * clearResources - Clears all resources in case of interruption.
 *
 * @signum: the signal number
 */
void clearResources(int signum) {
  cleanUp();
  exit(0);
}

//===============================
// IPC Functions
//===============================

/**
 * intiSchGenCom - Initializes the message queue between the scheduler and the
 * process generator.
 *
 * return: the message queue id
 */
int initSchGenCom() {
  int key = ftok("keyfiles/SCH_GEN_COM", SCH_GEN_COM);
  int msgQID = msgget(key, 0666 | IPC_CREAT);

  if (msgQID == -1) {
    perror("Error in creating message queue");
    exit(-1);
  }

  return msgQID;
}

void up(int semid) {
  struct sembuf sembuff;
  sembuff.sem_num = 0;
  sembuff.sem_op = 1;
  sembuff.sem_flg = !IPC_NOWAIT;

  if (semop(semid, &sembuff, 1) == -1) {
    perror("Error in up operation");
    exit(-1);
  } else if (DEBUG) {
    printf("up operation performed successfully with semid = %d\n", semid);
  }
}

void down(int semid) {
  struct sembuf sembuff;
  sembuff.sem_num = 0;
  sembuff.sem_op = -1;
  sembuff.sem_flg = !IPC_NOWAIT;

  if (semop(semid, &sembuff, 1) == -1) {
    perror("Error in down operation");
    exit(-1);
  } else if (DEBUG) {
    printf("Down operation performed successfully of semid = %d\n", semid);
  }
}

int initSchProShm(int pid) {
  int id2 = ftok("keyfiles/PRO_SCH_SHM", pid);
  int shmid = shmget(id2, sizeof(int), IPC_CREAT | 0666);

  if (shmid == -1) {
    perror("error in creating shared memory\n");
    exit(-1);
  } else if (DEBUG) {
    printf("shmadd created successfully\n");
  }

  return shmid;
}

int initSchProQ() {
  int id = ftok("keyfiles/PRO_SCH_Q", SCH_PRO_COM);
  int q_id = msgget(id, IPC_CREAT | 0666);

  if (q_id == -1) {
    perror("error in creating msg queue between process & scheduler");
    exit(-1);
  } else if (DEBUG) {
    printf("Message queue created successfully with pid = %d\n", q_id);
  }

  return q_id;
}

int initSchProSem() {
  int id = ftok("keyfiles/PRO_SCH_SEM", SCH_PRO_COM);
  int semid = semget(id, 1, 0666 | IPC_CREAT);

  if (semid == -1) {
    perror("Error in create sem\n");
    exit(-1);
  }

  return semid;
}
