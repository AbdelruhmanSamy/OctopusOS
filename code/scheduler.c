#include "headers.h"

int main(int argc, char *argv[]) {
  initClk();

  // TODO implement the scheduler :)
  // upon termination release the clock resources.

  // init msgs queue with process generator

  while (1) {
    // TODO: change this later just for testing
    int key = ftok("SCH_GEN_COM", 18);
    int msgQID = msgget(key, 0666 | IPC_CREAT);

    if (msgQID == -1) {
      perror("Error in creating message queue");
      exit(-1);
    }

    process_t process;

    // FIXME: doesn't seem to be recieving the process check later
    int response = msgrcv(msgQID, &process, sizeof(process_t), 0, !IPC_NOWAIT);

    if (response == -1) {
      perror("Error in receiving process from process generator");
      exit(-1);
    } else {
      printf(ANSI_BLUE "==>SCH: Received process" ANSI_RESET);
    }
  }

  destroyClk(true);
}
