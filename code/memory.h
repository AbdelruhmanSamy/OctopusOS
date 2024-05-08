/**
 * memory_block_t
 * A struct to represent a memory block in the memory
 */
typedef struct memory_block {
  int size;
  int realSize;
  int start;
  int end;
  int processId;
  int isFree;

  // tree like pointers
  struct memory_block *parent;
  struct memory_block *left;
  struct memory_block *right;
} memory_block_t;

// =============================================================================
// INTERNAL FUNCTIONS
int highestPowerOf2(int x);
memory_block_t *initializeMemoryBlock(int size, int start, int end);
memory_block_t *findMemoryBlock(memory_block_t *root, int addr);

// API
memory_block_t *initMemory();
memory_block_t *allocateMemory(memory_block_t *root, int size, int processId);
void freeMemory(memory_block_t *root, int processId);

// output functions
void createMemoryLogFile();
void memoryLogger(int time, const char *message, int processId, int size,
                  int start, int end);
void fancyPrintTree(memory_block_t *root, int level);
void fancyPrintMemoryBar(memory_block_t *root);

// =============================================================================
//                   ANSI COLOR CODES
// =============================================================================
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_RESET "\x1b[0m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_WHITE "\x1b[37m"
#define ANSI_RED_BG "\x1b[41m"
#define ANSI_GREEN_BG "\x1b[42m"
#define ANSI_YELLOW_BG "\x1b[43m"
#define ANSI_BLUE_BG "\x1b[44m"
#define ANSI_MAGENTA_BG "\x1b[45m"
#define ANSI_CYAN_BG "\x1b[46m"
#define ANSI_WHITE_BG "\x1b[47m"
#define ANSI_BLACK "\x1b[30m"
