#include "headers.h"
#include <unistd.h>

int *RT;
int shmid;
int semid;
void sigTermHandler(int signum) {
  if (shmdt(RT) == -1) {
    perror("Error in detach\n");
  } else if (DEBUG) {
    printf(ANSI_GREEN
           "process %d detached from memory successfully\n" ANSI_RESET,
           getpid());
  }

  if (shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) == -1) {
    perror("error in deleting shared memory");
  }

  printf(ANSI_GREY "==>process %d: Terminating\n" ANSI_RESET, getpid());
  exit(getpid());
}

int main(int agrc, char *argv[]) {

  signal(SIGTERM, sigTermHandler);

  // initailizing remaining time shared memory
  shmid = initSchProShm(getpid());
  RT = (int *)shmat(shmid, (void *)0, 0);

  semid = initSchProSem();
  
  initClk();

  printf(ANSI_TEAL "==>process %d: Started\n" ANSI_RESET, getpid());

  int currTime = getClk();
  int preTime = currTime;

  down(semid);
  int val = *RT;
  up(semid);

  while (val > 0) {

    if (currTime != preTime) {
      preTime = currTime;
      
      down(semid);
      (*RT)--;
      val = *RT;
      up(semid);
      
    }

    currTime = getClk();
  }

  kill(getppid(), SIGUSR1);

  destroyClk(false);

  raise(SIGTERM);
  return 0;
}
