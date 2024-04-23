#pragma once
#include "structs.h"
#include <stdlib.h>

/**
 * @list: array of data
 * @size: number of elements in the heap
 * @compare:
 returns:
  -  1: element1 > element2
  - -1: element1 < element2
  -  0: element1 = element2

 * Description: minimum heap structure
*/
typedef struct min_heap {
  size_t capacity;
  void **arr;
  size_t size;
  int (*compare)(void *e1, void *e2);
} min_heap;

// returns pointer on the heap to start using it
min_heap *createMinHeap(int comp(void *, void *));
// insert new element in the heap
void insertMinHeap(min_heap **heap, void *element);
// get the min element
void *extractMin(min_heap *heap);
void *getMin(min_heap *heap);
// shift-up operation, called on decreasing the priority value
void decreaseKey(min_heap *heap, int ind);
void minHeapify(min_heap *heap, int ind);
min_heap **doubleCapacity(min_heap *heap);
void swap(void **arr, int ind1, int ind2);
void printHeap(min_heap *heap);
