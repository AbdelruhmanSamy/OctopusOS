#include "queue.h"
#include "list.h"
#include <stdio.h>

/**
 * create a new queue
 *
 * return a pointer to the queue, NULL on failure
 */
queue *createQueue(void (*free_func)(void *)) {
  queue *newQueue = malloc(sizeof(*newQueue));

  if (!newQueue)
    return NULL;

  newQueue->list = createList(free_func);

  if (!newQueue->list)
    return NULL;

  return newQueue;
}

/**
 * return 1 if the queue is empty or not created, 0 otherwise
 */
int empty(queue *q) {
  if (!q)
    return 1; //! may need to change this

  return (q->list->size == 0);
}

/**
 * get number of elements in queue
 */
size_t size(queue *q) { return q->list->size; }

/**
 * add an item to the end of the queue
 * @q: pointer to queue
 * @item: element to add (pointer)
 */
void push(queue *q, void *item) {
  if (!q)
    return;

  insertNodeEnd(q->list, item);
}

/**
 * remove the first element from the queue
 * @q: pointer to queue
 */
void *pop(queue *q) {
  void *n;

  if (empty(q))
    return NULL;

  n = getNode(q->list, 0)->data;
  deleteNodeStart(q->list);
  return n;
}

/**
 * return the front element of the queue
 * without removing it,
 * NULL if empty
 * @q: pointer to queue
 */
void *front(queue *q) {
  if (empty(q))
    return NULL;

  d_node *n = getNode(q->list, 0);
  return n->data;
}

/**
 * return the last added element to the queue
 * without removing it
 * NULL if empty
 * @q: pointer to queue
 */
void *back(queue *q) {
  if (empty(q))
    return NULL;

  d_node *n = getNode(q->list, q->list->size - 1);
  return n->data;
}

/**
 * clear all and free memory
 * @q: pointer to queue
 */
void destroyQueue(queue *q) {
  destroyList(&q->list);
  free(q);
}
