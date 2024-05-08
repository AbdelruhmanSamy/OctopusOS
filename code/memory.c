#include "memory.h"
#include <stdlib.h>

#define TOTAL_MEMORY_SIZE 1024
#define MAX_PROCESS_SIZE 256

memory_block_t *memory = NULL;

/**
 * Starts the main whole memory (root of the tree)
 */
void init_memory() {
  memory = malloc(sizeof(memory_block_t));
  memory->size = TOTAL_MEMORY_SIZE;
  memory->start = 0;
  memory->end = TOTAL_MEMORY_SIZE;
  memory->realSize = -1;
  memory->left = NULL;
  memory->right = NULL;
  memory->parent = NULL;
  memory->processId = -1;
  memory->isFree = 0;
}
