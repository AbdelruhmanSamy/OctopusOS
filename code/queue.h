#pragma once

#include "list.h"
#include <stdlib.h>

typedef struct queue {
  d_list *list;
} queue;

queue *createQueue(void (*free_func)(void *));
int empty(queue *q);
size_t size(queue *q);
void push(queue *q, void *item);
void *pop(queue *q);
void *front(queue *q);
void *back(queue *q);
void destroyQueue(queue *q);
