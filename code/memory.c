#include "memory.h"
#include <stdlib.h>

#define TOTAL_MEMORY_SIZE 1024
#define MAX_PROCESS_SIZE 256

memory_block_t *memory = NULL;
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

