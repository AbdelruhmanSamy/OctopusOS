#include "headers.h"

/* Modify this file as needed*/
int remainingtime = 1;

int main(int agrc, char *argv[]) {

  signal(SIGINT, clearResources);
  signal(SIGTERM, clearResources);
  if (atexit(cleanUp) != 0) {
    perror("atexit");
    exit(1);
  }

  initClk();

  // TODO it needs to get the remaining time from somewhere
  // remainingtime = ??;
  while (remainingtime > 0) {
    // remainingtime = ??;
  }

  destroyClk(false);

  return 0;
}
