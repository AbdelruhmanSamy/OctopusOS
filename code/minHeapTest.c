#include "minHeap.h"
#include <stdio.h>

int compare(void* a , void* b){
    if((int*)a > (int*)b)
        return 1;
    if((int*)a < (int*)b)
        return -1;
    return 0;

}

int main(int argc , char* argv[]){
    
    min_heap* newHeap = createMinHeap(&compare);
    min_heap** newHeapRef = &newHeap;
    insertMinHeap(newHeapRef , (void*)5);
    insertMinHeap(newHeapRef , (void*)3);
   
    insertMinHeap(newHeapRef , (void*)6);
    insertMinHeap(newHeapRef , (void*)9);
    insertMinHeap(newHeapRef , (void*)0);

    printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));
    
    printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));
    printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));
    
    return 0;
}

