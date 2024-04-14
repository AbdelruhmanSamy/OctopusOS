#pragma once

#include <stdlib.h>

#define FORWARD 1
#define BACKWARD 2

/**
 * struct d_node - doubly linked node
 * @data: integer
 * @prev: pointer to the previous node
 * @next: pointer to the next node
 *
 * Description: doubly linked list node structure
 */
typedef struct d_node {
  void *data;
  struct d_node *prev;
  struct d_node *next;
} d_node;

/**
 * struct d_list - doubly linked list
 * @size: size of the linked list
 * @head: pointer to head node
 * @tail: pointer to tail node
 *
 * Description: doubly linked list structure
 */
typedef struct d_list {
  size_t size;
  void (*free_func)(void *);
  d_node *head;
  d_node *tail;
} d_list;

d_list *createList(void (*free_func)(void *));
d_node *getNode(d_list *list, unsigned int index);
d_node *insertNodeStart(d_list *list, void *n);
d_node *insertNodeEnd(d_list *list, void *n);
d_node *insertNode(d_list *list, unsigned int index, void *n);
int deleteNodeStart(d_list *list);
int deleteNodeEnd(d_list *list);
int deleteNode(d_list *list, unsigned int index);
void freeNode(d_list *list, d_node *node);
void freeList(d_list *list);
void destroyList(d_list **list);
