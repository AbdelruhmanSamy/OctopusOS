#include "headers.h"
#include <unistd.h>

int *shmAdd;
int shmid;

void sigTermHandler(int signum) {
  if (shmdt(shmAdd) == -1) {
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

  shmid = initSchProShm(getpid());
  shmAdd = (int *)shmat(shmid, (void *)0, 0);

  initClk();

  printf(ANSI_TEAL "==>process %d: Started\n" ANSI_RESET, getpid());

  int currTime = getClk();
  int preTime = currTime;
  while (*shmAdd > 0) {

    if (currTime != preTime) {
      preTime = currTime;
      (*shmAdd)--;
    }

    currTime = getClk();
  }

  kill(getppid(), SIGUSR1);

  destroyClk(false);

  raise(SIGTERM);
  return 0;
}
