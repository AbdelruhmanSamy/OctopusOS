#include "memory.h"

#define TOTAL_MEMORY_SIZE 1024
#define MAX_PROCESS_SIZE 256

// NOTE: this struct represents the whole memory
// You may considerit an array implementation of a binary heap
memory_block_t memory[TOTAL_MEMORY_SIZE];
