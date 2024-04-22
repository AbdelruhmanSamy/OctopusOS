#include "headers.h"
#include "structs.h"
#include "scheduler.h"

/* Modify this file as needed*/
int remainingtime = 1;


// TODO: send message from process to sch
void sendMessToSch(char* msg , int q_id){
  // TODO: implement
}


void recievePCBFromSch(PCB_t* p) {
  int id = ftok("keyfiles/PRO_SCH_Q", getpid());
  int q_id = msgget(id, IPC_CREAT | 0666);

  if (q_id == -1) {
    perror("Error in creating queue\n");
    exit(-1);
  } else if (DEBUG) {
    printf("q_id = %d\n", q_id);
  }
  fflush(stdout);

  struct msgbuff msgbuffer;
  int res = msgrcv(q_id, &msgbuffer, sizeof(PCB_t*), getpid(), !IPC_NOWAIT);

  if (res == -1) {
    perror("Error in receiving pcb\n");
    exit(-1);
  } else if (DEBUG) {
    printf("PCB received successfully\n");
    printf("Inside recievePCBFromSch, BT = %d\n", msgbuffer.pcb->process.BT);
  }

  // Copy received PCB data to the p pointer
  memcpy(&p, &msgbuffer.pcb, sizeof(PCB_t*));
}

int main(int agrc, char *argv[]) {

  printf("In process with pid = %d\n" , getpid());
  fflush(stdout);

  signal(SIGINT, clearResources);
  signal(SIGTERM, clearResources);
  if (atexit(cleanUp) != 0) {
    perror("atexit");
    exit(1);
  }

  int semid = atoi(argv[1]);
  if(DEBUG)
    printf("In process %d, semid = %d\n" , getpid(), semid);
  

  PCB_t* p ;
  recievePCBFromSch(p);
  
  if(DEBUG)
    printf("In process %d, BT = %d\n" , getpid() , (p)->process.BT);
  
  fflush(stdout);
  sleep(5);
  
  initClk();

  int preTime = getClk();
  // TODO it needs to get the remaining time from somewhere
  // remainingtime = ??;
  while (remainingtime > 0) {
    // remainingtime = ??;
    down(semid);
   
    int currTime = getClk();
    
    if(currTime != preTime){
      preTime = currTime;
      // if(--process->BT == 0)
      //   raise(SIGTERM);
      // if(DEBUG)
      //   printf("In process %d, BT = %d , curr state is RUNNING" , getpid() , process->BT);
    }
      
    up(semid);

  }

  destroyClk(false);

  return 0;
}
