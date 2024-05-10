/* ====================================================================
 * process_generator.c
 *
 * The process generator is responsible for reading the input file and
 * sending the processes to the scheduler at the appropriate time.
 * assigned to: amir-kedis
 * ====================================================================
 */

#include "process_generator.h"
#include "headers.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "./GUI/raygui.h"
#include <string.h>

#undef RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "./GUI/gui_window_file_dialog.h"

#define WID_WIDTH 1080
#define WID_HEIGHT 720

/**
 * main - The main function of the process generator.
 *
 * @argc: the number of arguments
 * @argv: the arguments
 * return: 0 on success, -1 on failure
 */
int main(int argc, char *argv[]) {
  queue *processes;
  scheduler_type schedulerType;
  int quantum = 0;
  pid_t schedulerPid, clockPid;
  int msgQID;
  char *fileName = "processes.txt";

  signal(SIGINT, clearResources);
  signal(SIGTERM, clearResources);

  InitWindow(WID_WIDTH, WID_HEIGHT, "OctopusOS");
  GuiLoadStyle("GUI/style_cyber.rgs");
  SetTargetFPS(60);

  printBanner();

  if (!GUI)
    processes = readInputFile(fileName);

  processes = getInput(&schedulerType, &quantum);

  printf(ANSI_GREEN "number of processes: %ld\n" ANSI_RESET, size(processes));
  createSchedulerAndClock(&schedulerPid, &clockPid, (int)schedulerType,
                          quantum);

  initClk();
  msgQID = initSchGenCom();

  sendProcessesToScheduler(processes, msgQID);
  destroyQueue(processes);

  wait(NULL);

  CloseWindow();
  cleanUp();
}

/**
 * readInputFile - Reads the input file and stores the data in the data
 * structure.
 *
 * return: a pointer to the queue of processes
 */
queue *readInputFile(char *fileName) {
  FILE *file;
  char *line = NULL;
  size_t len = 0;
  queue *processes = createQueue(free);

  printf(ANSI_YELLOW "============================" ANSI_RESET "\n");
  printf(ANSI_YELLOW "|| Reading processes file ||" ANSI_RESET "\n");
  printf(ANSI_YELLOW "============================" ANSI_RESET "\n");

  file = fopen(fileName, "r");
  if (file == NULL) {
    perror("Error in opening file");
    exit(-1);
  }

  while (getline(&line, &len, file) != -1) {

    if (line[0] == '#') {
      continue;
    }
    process_t *newProcess = malloc(sizeof(process_t));
    sscanf(line, "%d %d %d %d %d", &newProcess->id, &newProcess->AT,
           &newProcess->BT, &newProcess->priority, &newProcess->memsize);
    push(processes, newProcess);
    printf(ANSI_PURPLE
           "=>Inserted process with id: %d, AT: %d, BT: %d, "
           "priority: %d, memsize: %d into processes queue\n" ANSI_RESET,
           newProcess->id, newProcess->AT, newProcess->BT, newProcess->priority,
           newProcess->memsize);
    // free(line);
  }

  fclose(file);

  printf(ANSI_YELLOW "============================" ANSI_RESET "\n");
  printf(ANSI_YELLOW "|| Finished reading file   ||" ANSI_RESET "\n");
  printf(ANSI_YELLOW "============================" ANSI_RESET "\n");

  return processes;
}

/**
 * getSchedulerType - Asks the user to choose the scheduling algorithm to start
 * the simulation.
 *
 * return: the chosen scheduler type
 */
scheduler_type getSchedulerType() {
  int choice;

  printf(ANSI_TEAL "==========================================================="
                   "=\n" ANSI_RESET);
  printf(
      ANSI_TEAL
      "|| Choose the scheduling algorithm to start the simulation ||" ANSI_RESET
      "\n");
  printf(ANSI_TEAL "==========================================================="
                   "=\n" ANSI_RESET);

  printf(ANSI_GREEN
         "1. Non-preemptive Highest Priority First (HPF)\n" ANSI_RESET);
  printf(ANSI_GREEN "2. Shortest Remaining Time Next (SRTN)\n" ANSI_RESET);
  printf(ANSI_GREEN "3. Round Robin (RR)\n" ANSI_RESET);
  scanf("%d", &choice);

  switch (choice) {
  case 1:
    return HPF;
  case 2:
    return SRTN;
  case 3:
    return RR;
  default:
    exit(-1);
  }
}

/**
 * getInput - Asks the user for the chosen scheduling algorithm and its
 * parameters, if there are any.
 *
 * @schedulerType: a pointer to the chosen scheduler type
 * @quantum: a pointer to the quantum value
 */
queue *getInput(scheduler_type *schedulerType, int *quantum) {
  if (GUI) {
    *schedulerType = HPF;
    *quantum = 0;
    int dropdownBoxActive = 0;
    int dropdownBoxItemsCount = 3;
    char *dropdownBoxoptions = "#01#HPF;#02#SRTN;#03#RR";

    int *sliderValue = quantum;
    int sliderActive = 0;

    GuiWindowFileDialogState fileDialogState =
        InitGuiWindowFileDialog(GetWorkingDirectory());
    bool fileExitWindow = false;
    char *fileNameToLoad = malloc(100);
    fileNameToLoad[0] = '\0';

    while (true) {

      if (fileDialogState.SelectFilePressed) {
        strcpy(fileNameToLoad,
               TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText,
                          fileDialogState.fileNameText));

        fileDialogState.SelectFilePressed = false;
        printf("Selected file: %s\n", fileNameToLoad);
      }

      BeginDrawing();

      ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

      // Title
      GuiSetStyle(DEFAULT, TEXT_SIZE, 60);
      GuiLabel((Rectangle){350, 160, 400, 40}, "OctopusOS");

      // caption
      GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
      GuiLabel((Rectangle){300, 200, 500, 40}, "Please choose your params");

      // quantum spinner
      if (*schedulerType == RR) {
        GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
        GuiLabel((Rectangle){400, 350, 300, 40}, "Quantum");

        if (GuiSpinner((Rectangle){400, 400, 300, 40}, NULL, sliderValue, 1,
                       100, sliderActive) != 0)
          sliderActive = !sliderActive;

        *quantum = *sliderValue;
      }

      // choose file
      if (fileDialogState.windowActive)
        GuiLock();
      GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
      int ReadFile = GuiButton((Rectangle){400, 450, 300, 40}, "Choose File");
      if (ReadFile) {
        fileDialogState.windowActive = true;
      }
      if (fileNameToLoad != NULL) {
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        GuiLabel((Rectangle){250, 500, 600, 40}, fileNameToLoad);
      }
      GuiUnlock();

      // dropdown box
      GuiSetStyle(DEFAULT, TEXT_SIZE, 30);

      if (GuiDropdownBox((Rectangle){400, 300, 300, 40}, dropdownBoxoptions,
                         (int *)schedulerType, dropdownBoxActive)) {
        dropdownBoxActive = !dropdownBoxActive;
      }

      GuiSetStyle(DEFAULT, TEXT_SIZE, 35);
      int res = GuiButton((Rectangle){400, 550, 300, 40}, "Start");

      GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
      GuiWindowFileDialog(&fileDialogState);

      if (res == 1) {
        printf("Selected: %d\n", *schedulerType);
        printf("Quantum: %d\n", *quantum);
        printf("File: %s\n", fileNameToLoad);

        return readInputFile(fileNameToLoad);
      }

      EndDrawing();
    }

    return NULL;
  }

  *schedulerType = getSchedulerType();

  if (*schedulerType == -1) {
    printf(ANSI_RED "Invalid choice, try again...\n" ANSI_RESET);
    return getInput(schedulerType, quantum);
  }

  if (*schedulerType == RR) {
    printf(ANSI_YELLOW "Enter the quantum value: " ANSI_RESET);
    scanf("%d", quantum);

    if (*quantum <= 0) {
      printf(ANSI_RED "Invalid quantum value, try again...\n" ANSI_RESET);
      return getInput(schedulerType, quantum);
    }
  }

  char *schedulerTypeStr;

  switch (*schedulerType) {
  case HPF:
    schedulerTypeStr = "HPF";
    break;
  case SRTN:
    schedulerTypeStr = "SRTN";
    break;
  case RR:
    schedulerTypeStr = "RR";
    break;
  default:
    schedulerTypeStr = "Unknown";
    break;
  }

  printf(ANSI_GREEN "============================\n" ANSI_RESET);
  printf(ANSI_GREEN "||       PARAMETERS       ||\n" ANSI_RESET);
  printf(ANSI_GREEN "============================\n" ANSI_RESET);
  printf(ANSI_GREEN "|| Scheduler type | %4s  ||\n" ANSI_RESET,
         schedulerTypeStr);
  if (*schedulerType == RR) {
    printf(ANSI_GREEN "|| Quantum:       |   %d   ||\n" ANSI_RESET, *quantum);
  }
  printf(ANSI_GREEN "============================\n" ANSI_RESET);
}

/**
 * printBanner - Prints the banner of the process generator.
 */
void printBanner() {
  printf(ANSI_PURPLE);
  printf("                              ___\n"
         "                           .-'   `'.\n"
         "                          /         \\\n"
         "                          |         ;\n"
         "                          |         |           ___.--,\n"
         "                 _.._     |0) ~ (0) |    _.---'`__.-( (_.\n"
         "          __.--'`_.. '.__.\\    '--. \\_.-' ,.--'`     \\\"\\\"\"\n"
         "         ( ,.--'`   ',__ /./;   ;, '.__.'`    __\n"
         "         _`) )  .---.__.' / |   |\\   \\__..--\"\"\"\"  \"\"--.,_\n"
         "        `---' .'.''-._.-'`_./  /\\ '.  \\ _.-~~~````~~~-._`-.__.'\n"
         "              | |  .' _.-' |  |  \\  \\  '.               `~---`\n"
         "               \\ \\/ .'     \\  \\   '. '-._)\n"
         "                \\/ /        \\  \\    `=.__`~-.\n"
         "                / /\\         `) )    / / \\\"\\\"\".`\\\n"
         "          , _.-'.'\\ \\        / /    ( (     / /\n"
         "           `--~`   ) )    .-'.'      '.'.  | (\n"
         "                  (/`    ( (`          ) )  '-;\n"
         "                   `      '-;         (-'\n");

  printf(
      "________          __                             ________    "
      "_________ \n"
      "\\_____  \\   _____/  |_  ____ ______  __ __  _____\\_____  \\  /   "
      "_____/ \n"
      " /   |   \\_/ ___\\   __\\/  _ \\____ \\|  |  \\/  ___//   |   \\ "
      "\\_____ "
      " \\  \n"
      "/    |    \\  \\___|  | (  <_> )  |_> >  |  /\\___ \\/    |    \\/      "
      "  \\ \n"
      "\\_______  /\\___  >__|  \\____/|   __/|____//____  >_______  /_______ "
      " / \n"
      "        \\/     \\/            |__|              \\/        \\/        "
      "\\/  \n");

  if (DELAY)
    sleep(1);
  printf("Welcome to OctopusOS\n");
  if (DELAY)
    sleep(1);

  // GUI Banner
  while (true) {
    BeginDrawing();

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    DrawText("Welcome to OctopusOS", 280, 300, 48,
             GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    if (GuiButton((Rectangle){400, 400, 300, 40}, "Start Simulation")) {
      break;
    }

    EndDrawing();
  }
  printf(ANSI_RESET);
}

/**
 * createSchedulerAndClock - Creates the scheduler and clock processes.
 *
 * @schedulerPid: a pointer to the scheduler process id
 * @clockPid: a pointer to the clock process id
 */
void createSchedulerAndClock(pid_t *schedulerPid, pid_t *clockPid,
                             int schedulerType, int quantum) {

  *clockPid = fork();

  if (*clockPid == -1) {
    perror("Error in creating clock process");
    exit(-1);
  }

  if (*clockPid == 0) {
    char *args[] = {"./clk.out", NULL};
    execvp(args[0], args);
    exit(0);
  }

  *schedulerPid = fork();

  if (*schedulerPid == -1) {
    perror("Error in creating scheduler process");
    exit(-1);
  }

  if (*schedulerPid == 0) {
    char type[2];
    char q[5];

    sprintf(type, "%d", schedulerType);
    sprintf(q, "%d", quantum);

    char *args[] = {"./scheduler.out", type, q, NULL};
    execvp(args[0], args);
    exit(0);
  }

  printf(ANSI_PURPLE "|| ==>   Created Scheduler and Clock\n" ANSI_RESET);
  printf(ANSI_PURPLE "||    ==> Scheduler PID: %d\n" ANSI_RESET, *schedulerPid);
  printf(ANSI_PURPLE "||    ==> Clock     PID: %d\n" ANSI_RESET, *clockPid);
}

/**
 * sendProcessesToScheduler - Sends the processes to the scheduler at the
 * appropriate time.
 *
 * @processes: a pointer to the queue of processes
 */
void sendProcessesToScheduler(queue *processes, int msgQID) {
  int currentTime = getClk();
  int lastTime = currentTime;
  process_t *process;
  int response;
  int Count = size(processes);
  float *progress = malloc(sizeof(float) * Count);

  while (!empty(processes)) {
    process = (process_t *)front(processes);
    currentTime = getClk();

    BeginDrawing();

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    DrawText("Sending Processes to Scheduler", 150, 300, 48,
             GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

    *progress = Count - size(processes);
    GuiProgressBar((Rectangle){150, 400, 800, 40}, NULL, NULL, progress, 0,
                   Count);

    EndDrawing();

    if (currentTime < process->AT) {
      lastTime = currentTime;
      continue;
    }

    printf(ANSI_PURPLE "=>GEN:Sending process with id: %d, AT: %d, BT: %d, "
                       "priority: %d, memzie: %d to scheduler\n" ANSI_RESET,
           process->id, process->AT, process->BT, process->priority,
           process->memsize);

    process->RT = malloc(sizeof(int) * process->BT);
    *process->RT = process->BT;
    process->WT = 0;
    process->TA = 0;
    process->LST = currentTime;
    process->ST = -1;
    process->FT = -1;
    process->state = READY;
    process->pid = 1;

    response = msgsnd(msgQID, process, sizeof(process_t), !IPC_NOWAIT);
    if (response == -1) {
      perror("Error in sending process to scheduler\n");
      exit(-1);
    }

    pop(processes);
    lastTime = currentTime;
  }
  BeginDrawing();

  ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

  DrawText("Sent All Processes to Scheduler", 150, 300, 48,
           GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

  *progress = Count;
  GuiProgressBar((Rectangle){150, 400, 800, 40}, NULL, NULL, progress, 0,
                 Count);

  DrawText("Check other window for output", 150, 500, 48,
           GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

  EndDrawing();

  process->id = -1;
  response = msgsnd(msgQID, process, sizeof(process_t), !IPC_NOWAIT);
  if (response == -1) {
    perror("Error in terminating sending processes to scheduler\n");
    exit(-1);
  }
}
