#pragma once
#include "structs.h"
// #include "headers.h"
#include <stdlib.h>

/**
 * @list: array of data
 * @size: number of elements in the heap
 * @compare: 
 returns: 
  -  1: element1 > element2
  - -1: element1 < element1
  -  0: element1 = element2
  
 * Description: minimum heap structure
*/
typedef struct min_heap{
    size_t capacity;      
    void** arr;        
    size_t size;
    int (*compare)(void* e1, void* e2);
}min_heap;

min_heap* createMinHeap(int comp(void* , void* ));  //returns pointer on the heap to start using it
void insertMinHeap(min_heap** heap , void* element);                                  //insert new element in the heap
void* extractMin(min_heap* heap);                                  //get the min element
void decreaseKey(min_heap* heap ,int ind);          //shift-up operation, called on decreasing the priority value
void minHeapify(min_heap* heap , int ind);            
min_heap** doubleCapacity(min_heap* heap);
void swap(void** arr, int ind1, int ind2);
void printHeap(min_heap* heap); 