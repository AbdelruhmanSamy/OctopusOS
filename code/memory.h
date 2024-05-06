// NOTE: this file and the API I created is subject to change when we progress
// In my planning when we implement this the scheduler change will be just a few
// function calls
/**
 * memory_block_t
 * A struct to represent a memory block in the memory
 * size: the size of the memory block
 * isFree: a flag to indicate if the memory block is free or not
 * buddy: a pointer to the buddy of the memory block
 */
typedef struct memory_block {
  int size;
  int isFree;
  struct memory_block *buddy;
} memory_block_t;

// internal functions
memory_block_t *findFreeBlock(int size);
void mergeBuddies(memory_block_t *block);

// output functions
void memoryLogger(int time, const char *message, int processId, int size,
                  int start, int end);
void fancyPrintMemory();

// The following functions are the API for the memory management
void initMemory();
void allocateMemory(memory_block_t *block, int size, int processId);
void freeMemory(memory_block_t *block, int processId);
