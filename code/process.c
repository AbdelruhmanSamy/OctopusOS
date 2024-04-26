#include "headers.h"
#include "scheduler.h"
#include "structs.h"

int *shmAdd;
int shmid;

void sigTermHandler(int signum) {
  if (shmdt(shmAdd) == -1) {
    perror("Error in detach\n");
  } else if (DEBUG) {
    printf(ANSI_GREEN
           "process %d detached from memory sucessfully\n" ANSI_RESET,
           getpid());
  }

  if (shmctl(shmid, IPC_RMID, (struct shmid_ds *)0) == -1) {
    perror("error in deleting shared memory");
  }

  // FIXME: Samy look at these 2 lines
  // I added them because process was reaching return 0 and making the exit
  // cleanup which kills the clock and so the whole app
  // make sure it doesn't affect the process
  printf(ANSI_GREY "==>process %d: Terminating\n" ANSI_RESET, getpid());
  raise(SIGKILL);
}

int main(int agrc, char *argv[]) {

  fflush(stdout);

  signal(SIGINT, clearResources);
  signal(SIGTERM, sigTermHandler);

  if (atexit(cleanUp) != 0) {
    perror("atexit");
    exit(1);
  }

  shmid = initSchProShm(getpid());
  shmAdd = (int *)shmat(shmid, (void *)0, 0);

  fflush(stdout);
  initClk();

  printf(ANSI_TEAL "==>process %d: Started\n" ANSI_RESET, getpid());

  int preTime = getClk();
  while (*shmAdd > 0) {
    int currTime = getClk();

    if (currTime != preTime) {
      preTime = currTime;

      printf(ANSI_TEAL "==>process %d: RT = %d \n" ANSI_RESET, getpid(),
             *shmAdd);
      (*shmAdd)--;
    }
  }

  kill(getppid(), SIGUSR1);

  destroyClk(false);

  raise(SIGTERM);

  return 0;
}
