#pragma once

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h> //if you don't use scanf/printf change this include
#include <stdlib.h>
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
#define ANSI_CYAN "\x1b[36m"
#define ANSI_TEAL "\x1b[96m"
#define ANSI_WHITE "\x1b[37m"
#define ANSI_PURPLE "\x1b[35m"
#define ANSI_RESET "\x1b[0m"

#define short bool;
#define true 1
#define false 0

#define SCH_GEN_COM 5

#define SHKEY 300

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
  // TODO any other needed clean up
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
