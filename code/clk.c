/*
 * This file is done for you.
 * Probably you will not need to change anything.
 * This file represents an emulated clock for simulation purpose only.
 * It is not a real part of operating system!
 */

#include "headers.h"

int shmid;

/* Clear the resources before exit */
void cleanUpClk(int signum) {
  shmctl(shmid, IPC_RMID, NULL);
  printf(ANSI_YELLOW "==>Clock terminating!\n" ANSI_RESET);
  exit(0);
}

/* This file represents the system clock for ease of calculations */
int main(int argc, char *argv[]) {
  printf(ANSI_YELLOW "==>CLK: Clock starting\n" ANSI_RESET);
  signal(SIGINT, cleanUpClk);
  int clk = 0;
  // Create shared memory for one integer variable 4 bytes
  shmid = shmget(SHKEY, 4, IPC_CREAT | 0644);
  if ((long)shmid == -1) {
    perror("Error in creating shm!");
    exit(-1);
  }
  int *shmaddr = (int *)shmat(shmid, (void *)0, 0);
  if ((long)shmaddr == -1) {
    perror("Error in attaching the shm in clock!");
    exit(-1);
  }
  *shmaddr = clk; /* initialize shared memory */
  while (1) {
    printf(ANSI_YELLOW "=>CLK:Current time: %d\n" ANSI_RESET, (*shmaddr));
    sleep(1);
    (*shmaddr)++;
  }
}
