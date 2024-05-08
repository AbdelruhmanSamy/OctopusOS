#include "memory.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_MEMORY_SIZE 1024
#define LOG_FILE "memory.log"

/**
 * highestPowerOf2 - Find the highest power of 2 that is less than or equal to x
 *
 * @param x - The number
 * @return int - The highest power of 2
 */
int highestPowerOf2(int x) {
  if (x <= 0)
    return 1;

  int power = 1;
  while (power < x) {
    power *= 2;
  }
  return power;
}

/**
 * initMemory - Initialize the memory block
 *
 * @return memory_block_t* - The memory block
 */
memory_block_t *initMemory() {
  memory_block_t *memory = malloc(sizeof(memory_block_t));
  memory->size = TOTAL_MEMORY_SIZE;
  memory->start = 0;
  memory->end = TOTAL_MEMORY_SIZE;
  memory->realSize = -1;
  memory->left = NULL;
  memory->right = NULL;
  memory->parent = NULL;
  memory->processId = -1;
  memory->isFree = 1;
  return memory;
}

/**
 * initializeMemoryBlock - Initialize a memory block
 *
 * @param size - The size of the memory block
 * @param start - The start address of the memory block
 * @param end - The end address of the memory block
 * @return memory_block_t* - The memory block
 */
memory_block_t *initializeMemoryBlock(int size, int start, int end) {
  memory_block_t *block = malloc(sizeof(memory_block_t));

  if (block != NULL) {
    block->size = highestPowerOf2(size);
    block->realSize = size;
    block->start = start;
    block->end = end;
    block->processId = -1;
    block->isFree = 1;
    block->parent = NULL;
    block->left = NULL;
    block->right = NULL;
  }
  return block;
}

/**
 * allocateMemory - Allocate memory for a process
 *
 * @param root - The root of the memory block
 * @param size - The size of the memory block
 * @param processId - The process ID
 * @return memory_block_t* - The memory block
 */
memory_block_t *allocateMemory(memory_block_t *root, int size, int processId) {
  if (root == NULL)
    return NULL;

  if (root->isFree == 0)
    return NULL;

  if (root->size < highestPowerOf2(size))
    return NULL;

  if (!root->left && !root->right && root->size == highestPowerOf2(size)) {
    root->processId = processId;
    root->isFree = 0;
    return root;
  }

  memory_block_t *left = allocateMemory(root->left, size, processId);
  if (left != NULL)
    return left;

  if (root->size > highestPowerOf2(size)) {
    int leftSize = root->size / 2;

    if (root->left == NULL) {
      root->left =
          initializeMemoryBlock(leftSize, root->start, root->start + leftSize);
      root->left->parent = root;
      return allocateMemory(root->left, size, processId);
    } else if (root->right == NULL) {
      root->right =
          initializeMemoryBlock(leftSize, root->start + leftSize, root->end);
      root->right->parent = root;
      return allocateMemory(root->right, size, processId);
    }
  }

  memory_block_t *right = allocateMemory(root->right, size, processId);
  if (right != NULL)
    return right;
  return NULL;
}

/**
 * freeMemory - Free memory for a process
 *
 * @param root - The root of the memory block
 * @param processId - The process ID
 */
void freeMemory(memory_block_t *root, int processId) {
  if (root == NULL)
    return;

  if (root->processId == processId) {
    // remove me from tree
    if (root == root->parent->left) {
      root->parent->left = NULL;
    } else {
      root->parent->right = NULL;
    }

    root = root->parent;
    while (!root->left && !root->right && root->parent) {
      memory_block_t *parent = root->parent;
      if (parent->left == root) {
        parent->left = NULL;
      } else {
        parent->right = NULL;
      }
      root = parent;
    }

    return;
  }

  freeMemory(root->left, processId);
  freeMemory(root->right, processId);
}

/**
 * findMemoryBlock - Find a memory block by address used by printing
 *
 * @param root - The root of the memory block
 * @param addr - The address
 * @return memory_block_t* - The memory block
 */
memory_block_t *findMemoryBlock(memory_block_t *root, int addr) {
  if (root == NULL)
    return NULL;

  if (!root->left && !root->right && addr >= root->start && addr < root->end)
    return root;

  memory_block_t *left = findMemoryBlock(root->left, addr);
  if (left != NULL)
    return left;

  return findMemoryBlock(root->right, addr);
}

/**
 * findMemoryBlockByProcessId - Find a memory block by process ID
 *
 * @param root - The root of the memory block
 * @param processId - The process ID
 * @return memory_block_t* - The memory block
 */
memory_block_t *findMemoryBlockByProcessId(memory_block_t *root,
                                           int processId) {
  if (root == NULL)
    return NULL;

  if (root->processId == processId)
    return root;

  memory_block_t *left = findMemoryBlockByProcessId(root->left, processId);
  if (left != NULL)
    return left;

  return findMemoryBlockByProcessId(root->right, processId);
}

/**
 * fancyPrintTree - Print the memory layout in a fancy way
 *
 * @param root - The root of the memory block
 * @param level - The level of the memory block
 */
void fancyPrintTree(memory_block_t *root, int level) {
  if (root == NULL)
    return;

  printf("%s", root->isFree ? ANSI_GREEN : ANSI_RED);

  for (int i = 0; i < level; i++)
    printf("|  ");

  printf("[%d-%d] %s - Process ID: %d\n", root->start, root->end,
         root->isFree ? "Free" : "Allocated", root->processId);

  printf(ANSI_RESET);

  fancyPrintTree(root->left, level + 1);
  fancyPrintTree(root->right, level + 1);
}

/**
 * fancyPrintMemoryBar - Display the memory layout
 *
 * @param root - The root of the memory block
 */
void fancyPrintMemoryBar(memory_block_t *root) {
  if (root == NULL)
    return;

  printf(ANSI_BLACK);
  for (int addr = 0; addr < 1024; addr += 8) {
    memory_block_t *block = findMemoryBlock(root, addr);
    if (block != NULL && !block->isFree) {
      switch (block->processId % 6) {
      case 0:
        printf(ANSI_RED_BG);
        break;
      case 1:
        printf(ANSI_GREEN_BG);
        break;
      case 2:
        printf(ANSI_YELLOW_BG);
        break;
      case 3:
        printf(ANSI_BLUE_BG);
        break;
      case 4:
        printf(ANSI_MAGENTA_BG);
        break;
      case 5:
        printf(ANSI_CYAN_BG);
        break;
      default:
        printf(ANSI_RED_BG);
        break;
      }
      printf("%d", block->processId);
    } else {
      printf(ANSI_WHITE_BG " ");
    }
  }
  printf(ANSI_RESET);
  printf("\n");
}

/**
 * createMemoryLogFile - Create the memory log file
 */
void createMemoryLogFile() {
  FILE *logFileptr = fopen(LOG_FILE, "w");
  if (logFileptr == NULL) {
    perror("Can't Create Log File");
    exit(-1);
  }

  printf("Started Logging\n");
  fclose(logFileptr);
}

/**
 * memoryLogger - Log the memory allocation
 *
 * @param root - The root of the memory block
 * @param time - The time
 * @param message - The message
 * @param processId - The process ID
 * @param size - The size of the memory block
 */
void memoryLogger(memory_block_t *root, int time, const char *message,
                  int processId, int size) {
  FILE *file = fopen(LOG_FILE, "a");
  if (file == NULL)
    return;

  memory_block_t *block = findMemoryBlockByProcessId(root, processId);
  int start = block->start;
  int end = block->end;

  if (strcmp(message, "Allocated") == 0)
    fprintf(file, "At time %d %s %d bytes for process %d from %d to %d\n", time,
            message, size, processId, start, end);
  else
    fprintf(file, "At time %d %s %d bytes from process %d from %d to %d\n",
            time, message, size, processId, start, end);

  fclose(file);
}
// NOTE: left for testing uncomment to test
//
//  int main() {
//    createMemoryLogFile();
//    memory_block_t *memory = initMemory();
//    printf("Memory block size: %d\n", memory->size);
//    fancyPrintTree(memory, 0);
//    printf("\n");
//    allocateMemory(memory, 256, 1);
//    memoryLogger(0, "Allocated", 1, 256, 0, 256);
//    memoryLogger(2, "freed", 1, 256, 0, 256);
//    allocateMemory(memory, 32, 2);
//    allocateMemory(memory, 64, 3);
//    allocateMemory(memory, 128, 3);
//
//    allocateMemory(memory, 256, 4);
//    allocateMemory(memory, 40, 5);
//    allocateMemory(memory, 40, 6);
//    allocateMemory(memory, 8, 9);
//    fancyPrintTree(memory, 0);
//
//    printf("\n");
//    fancyPrintMemoryBar(memory);
//
//    freeMemory(memory, 4);
//    fancyPrintTree(memory, 0);
//    printf("\n");
//    fancyPrintMemoryBar(memory);
//    freeMemory(memory, 3);
//    fancyPrintTree(memory, 0);
//    printf("\n");
//    fancyPrintMemoryBar(memory);
//    freeMemory(memory, 1);
//    fancyPrintTree(memory, 0);
//    printf("\n");
//    fancyPrintMemoryBar(memory);
//    freeMemory(memory, 9);
//    fancyPrintTree(memory, 0);
//    printf("\n");
//    fancyPrintMemoryBar(memory);
//    freeMemory(memory, 2);
//    fancyPrintTree(memory, 0);
//    printf("\n");
//    fancyPrintMemoryBar(memory);
//    freeMemory(memory, 5);
//    freeMemory(memory, 6);
//    fancyPrintTree(memory, 0);
//    printf("\n");
//    fancyPrintMemoryBar(memory);
//    allocateMemory(memory, 256, 1);
//    allocateMemory(memory, 32, 2);
//    allocateMemory(memory, 64, 3);
//    allocateMemory(memory, 128, 3);
//
//    allocateMemory(memory, 256, 4);
//    allocateMemory(memory, 40, 5);
//    allocateMemory(memory, 40, 6);
//    allocateMemory(memory, 8, 9);
//    fancyPrintTree(memory, 0);
//    printf("\n");
//    fancyPrintMemoryBar(memory);
//    return 0;
//  }
