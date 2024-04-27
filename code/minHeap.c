#include "minHeap.h"
#include <stdbool.h>
#include <stdio.h>
/**
 * @comp: pass a function to be used to compare between any two heap elements
  return:
  -  1: element1 > element2
  - -1: element1 < element1
  -  0: element1 = element2
*/
min_heap *createMinHeap(int (*comp)(void *, void *)) {
  min_heap *newHeap = malloc(sizeof(*newHeap));
  newHeap->arr = malloc(sizeof(void *));
  newHeap->capacity = 10;
  newHeap->size = 0;
  newHeap->compare = comp;

  return newHeap;
}

/**
 * insert in the heap
 * pass a void pointer to the element you want to insert
 */
void insertMinHeap(min_heap **heap, void *element) {
  // if(DEBUG){
  // printf("entered insertion safely\n");
  // printHeap((*heap));
  // }
  //
  // FIXME: something here is not working size increae then decrease suddenly
  // when double capacity is needed

  if ((*heap)->size == (*heap)->capacity)
    (*heap) = *doubleCapacity((*heap));

  (*heap)->arr[(*heap)->size] = element;
  (*heap)->size++;

  decreaseKey((*heap), (*heap)->size - 1);
}

/**
 * get the min element in the heap
 */
void *extractMin(min_heap *heap) {
  // if(DEBUG){
  // printf("entered extraction!!\n");
  // printHeap(heap);
  // }

  void *minElement = heap->arr[0];
  heap->size--;
  heap->arr[0] = heap->arr[heap->size];
  heap->arr[heap->size] = 0;
  minHeapify(heap, 0);

  return minElement;
}

/**
 * get the min element in the heap
 */
void *getMin(min_heap *heap) {
  if (!heap || heap->size <= 0)
    return NULL;
  return heap->arr[0];
}

/**
 * Shift-up operation, called on decreasing the priority value
 */
void decreaseKey(min_heap *heap, int ind) {
  if (ind >= heap->size)
    return;

  void **arr = heap->arr;
  int parentInd = (ind - 1) / 2;

  while (parentInd >= 0 && heap->compare(arr[parentInd], arr[ind]) > 0) {
    swap(arr, ind, parentInd);
    ind = parentInd;
    parentInd = (ind - 1) / 2;
  }
}

/**
 * Shift-down operation, used locally
 */
void minHeapify(min_heap *heap, int ind) {
  int chInd = 2 * ind + 1, minInd = ind;

  bool flag = true;
  void **arr = heap->arr;

  while (flag && chInd < heap->size) {
    if (heap->compare(arr[ind], arr[chInd]) > 0)
      minInd = chInd;

    if (chInd + 1 < heap->size &&
        heap->compare(arr[minInd], arr[chInd + 1]) > 0)
      minInd = chInd + 1;

    if (minInd == ind)
      flag = false;
    else {
      swap(arr, ind, minInd);
      ind = minInd;
      chInd = 2 * ind + 1;
    }
  }
}

/**
 * Description: doubles the size of the array if the max capacity is reached.
 * used in insert function

 * - initializing new heap by double capacity
 * - copying data
 * - freeing memory of the old heap
*/
min_heap **doubleCapacity(min_heap *heap) {
  min_heap *newHeap = malloc(sizeof(*newHeap));
  newHeap->capacity = 2 * heap->capacity;
  newHeap->arr = calloc(newHeap->capacity, sizeof(void *));
  newHeap->size = heap->size;
  newHeap->compare = heap->compare;
  for (int i = 0; i < heap->size; i++)
    newHeap->arr[i] = heap->arr[i];

  min_heap **returnval = &newHeap;

  // if(DEBUG){
  // printf("doubling occurred!!\n");
  // printHeap(*returnval);
  // }

  return returnval;
}

void swap(void **arr, int ind1, int ind2) {
  void *temp = arr[ind1];
  arr[ind1] = arr[ind2];
  arr[ind2] = temp;
  return;
}

void printHeap(min_heap *heap) {
  // printf("=============================\n");
  // printf("capacity: %d \nsize: %d\n", (int)heap->capacity, (int)heap->size);
  // printf("=============================\n");
}
