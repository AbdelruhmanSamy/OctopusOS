#include "headers.h"
#include "structs.h"
#include "scheduler.h"

int* shmAdd;
int shmid;

void sigTermHandler(int signum){
  if(shmdt(shmAdd) == -1){
    perror("Error in detach\n");
  }
  else if(DEBUG){
    printf("process %d detached from memory sucessfully\n" , getpid());
  }

  if(shmctl(shmid , IPC_RMID , (struct shmid_ds*) 0)== -1 ){
    perror("error in deleting shared memory");
  }  
}

int main(int agrc, char *argv[]) {

  printf("In process with pid = %d\n" , getpid());
  fflush(stdout);

  signal(SIGINT, clearResources);
  signal(SIGTERM, sigTermHandler);

  if (atexit(cleanUp) != 0) {
    perror("atexit");
    exit(1);
  }
    
    shmid = initSchProShm(getpid());
    shmAdd = (int*)shmat(shmid , (void*)0 , 0);
    
    if(DEBUG)
      printf("In process %d , RT = %d\n" ,getpid(), *shmAdd);

  fflush(stdout);  
  initClk();

  int preTime = getClk();
  while (*shmAdd > 0) {
    int currTime = getClk();
    
    if(currTime != preTime){
      preTime = currTime;

      (*shmAdd)--;
      
      if(DEBUG)
        printf("In process %d, RT = %d \n" , getpid() , *shmAdd);
    }
  }

  kill(getppid() , SIGUSR1);

  destroyClk(false);

  raise(SIGTERM);

  return 0;
}
