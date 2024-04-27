#include "minHeap.h"
#include <stdio.h>

int compare(void* a , void* b){

    if((int*)a > (int*)b)
        return 1;
    if((int*)a < (int*)b)
        return -1;
    return 0;

}

int compareProcesses(void* p1 , void* p2){

    if(((process_t*)p1)->priority > ((process_t*)p2)->priority)
    return 1;
    if(((process_t*)p1)->priority < ((process_t*)p2)->priority)
    return -1;

    return 0;
}

void initializeProcess(process_t* p ,int at , int bt , int id , int pri ){
    p->AT = at; 
    p->BT = bt;
    p->id = id;
    p->priority = pri;
}


int main(int argc , char* argv[]){
    
    min_heap* newHeap = createMinHeap(compareProcesses);

    process_t p1;
    process_t p2;
    process_t p3;
    process_t p4;

    initializeProcess(&p1 , 5 , 6 ,1 , 4);
    initializeProcess(&p2 , 7 , 10 , 2 , 2);
    initializeProcess(&p3 , 7 , 10 , 3 , 8);
    initializeProcess(&p4 , 7 , 10 , 4 , 1);

    printf("size = %d\n", *(int*) &newHeap->size);
 
    insertMinHeap(&newHeap , &p3);
    printf("size = %d\n", *(int*) &newHeap->size);

    insertMinHeap(&newHeap , &p1);
    printf("size = %d\n", *(int*) &newHeap->size);

    insertMinHeap(&newHeap , &p4);
    printf("size = %d\n", *(int*) &newHeap->size);

    insertMinHeap(&newHeap , &p2);
    printf("size = %d\n", *(int*) &newHeap->size);


    printf("priority of the currentMin: %d \n" ,  ((process_t*)extractMin(newHeap))->priority);
    printf("size = %d\n", *(int*) &newHeap->size);
       
    printf("priority of the currentMin: %d \n" ,  ((process_t*)extractMin(newHeap))->priority);   
    printf("size = %d\n", *(int*) &newHeap->size);
    
    printf("priority of the currentMin: %d \n" ,  ((process_t*)extractMin(newHeap))->priority);   
    printf("size = %d\n", *(int*) &newHeap->size);

    printf("priority of the currentMin: %d \n" ,  ((process_t*)extractMin(newHeap))->priority);   
    printf("size = %d\n", *(int*) &newHeap->size);



    //Testing   integers
    /*
        insertMinHeap(newHeapRef , (void*)5);
        insertMinHeap(newHeapRef , (void*)3);
        insertMinHeap(newHeapRef , (void*)6);
        
        printf("currentMin without extraction: %d \n" ,  (newHeap)->arr[0]);
        printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));   
        
        insertMinHeap(newHeapRef , (void*)9);
        insertMinHeap(newHeapRef , (void*)2);

        printf("currentMin without extraction: %d \n" ,  (newHeap)->arr[0]);
        printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));
        
        printf("currentMin without extraction: %d \n" ,  (newHeap)->arr[0]);
        printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));
    

        printf("currentMin without extraction: %d \n" ,  (newHeap)->arr[0]);
        printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));

        printf("currentMin without extraction: %d \n" ,  (newHeap)->arr[0]);
        printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));

        printf("currentMin without extraction: %d \n" ,  (newHeap)->arr[0]);
        printf("currentMin: %d \n" ,  (int*)extractMin(newHeap));
    */
        
    return 0;
}

