#include "minHeap.h"
#include <stdio.h>

int compare(void* a , void* b){

    if((int*)a > (int*)b)
        return 1;
    if((int*)a < (int*)b)
        return -1;
    return 0;

}

void newFunc(int (*comp)(void* , void*)){
    printf("%d\n" , comp((void*) 99 ,(void*) 7));
}

int main(int argc , char* argv[]){
    
    min_heap* newHeap = createMinHeap(&compare);
    min_heap** newHeapRef = &newHeap;
    insertMinHeap(newHeapRef , (void*)5);
    insertMinHeap(newHeapRef , (void*)3);
   
    insertMinHeap(newHeapRef , (void*)6);
    //insertMinHeap(newHeap , (void*)9);
    //insertMinHeap(newHeap , (void*)0);

    printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));
    
    printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));
    printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));

   /*
   int (*cmp)(void* , void*) = &compare;
    //printf("%d\n",  cmp((void*)5 ,(void*)7));
    newFunc(cmp);
   */ 
   
    return 0;
}

