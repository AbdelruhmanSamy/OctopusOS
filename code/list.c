#include "list.h"
#include <stdio.h>

/**
 * createList - create a new d_list
 *
 * Return: pointer to new list, NULL on failure
 */
d_list *createList(void (*free_func)(void *)) {
  d_list *newList;

  newList = malloc(sizeof(*newList));
  if (!newList)
    return (NULL);

  newList->size = 0;
  newList->head = newList->tail = NULL;
  newList->free_func = free_func;

  return (newList);
}

/**
 * getNode - get the node in a given index
 * @list: pointer to list
 * @index: the index of the node
 *
 * Return: pointer to node at index
 */
d_node *getNode(d_list *list, unsigned int index) {
  d_node *ptr;
  int dir = FORWARD;
  unsigned int i = 0;

  if (!list)
    return (NULL);

  ptr = list->head;
  if (index > (list->size / 2)) {
    dir = BACKWARD;
    ptr = list->tail;
    i = list->size - 1;
  }

  while (ptr) {
    if (i == index)
      return (ptr);

    if (dir == FORWARD) {
      i++;
      ptr = ptr->next;
    } else {
      i--;
      ptr = ptr->prev;
    }
  }
  return (NULL);
}

/**
 * insertNodeStart - inserts a new node at the beginning of a d_list
 * @list: pointer to list
 * @n: value of the new node
 *
 * Return: pointer to new node, NULL in failure
 */
d_node *insertNodeStart(d_list *list, void *n) {
  d_node *newNode;

  if (!list)
    return (NULL);

  newNode = malloc(sizeof(*newNode));
  if (!newNode)
    return (NULL);

  newNode->data = n, newNode->prev = NULL;
  newNode->next = list->head;

  if (list->head)
    list->head->prev = newNode;
  else
    list->tail = newNode;
  list->head = newNode;
  list->size++;

  return (newNode);
}

/**
 * insertNodeEnd - inserts a new node at the end of a d_list
 * @list: pointer to list
 * @n: value of the new node
 *
 * Return: pointer to new node, NULL in failure
 */
d_node *insertNodeEnd(d_list *list, void *n) {
  d_node *newNode;

  if (!list)
    return (NULL);

  newNode = malloc(sizeof(*newNode));
  if (!newNode)
    return (NULL);

  newNode->data = n, newNode->next = NULL;
  newNode->prev = list->tail;

  if (list->tail)
    list->tail->next = newNode;
  else
    list->head = newNode;
  list->tail = newNode;
  list->size++;

  return (newNode);
}

/**
 * insertNode - inserts a new node at a given position
 * @list: pointer to list
 * @index: the index where the new node should be added
 * @n: value in the new node
 *
 * Return: pointet to new node, NULL in failure
 */
d_node *insertNode(d_list *list, unsigned int index, void *n) {
  d_node *newNode, *ptr;

  if (!list)
    return (NULL);
  if (!index)
    return (insertNodeStart(list, n));
  if (index == list->size)
    return (insertNodeEnd(list, n));
  if (index > list->size)
    return (NULL);

  ptr = getNode(list, index);
  newNode = malloc(sizeof(*newNode));
  if (!newNode)
    return (NULL);

  newNode->data = n;
  newNode->next = ptr;
  newNode->prev = ptr->prev;
  newNode->prev->next = newNode;
  newNode->next->prev = newNode;
  list->size++;

  return (newNode);
}

/**
 * deleteNodeStart - delete the node at the beginning of a d_list
 * @list: pointer to list
 *
 * Return: 1 on success, 0 on failure
 */
int deleteNodeStart(d_list *list) {
  d_node *toRemove;

  if (!list)
    return (0);
  if (!list->size)
    return (0);

  toRemove = list->head;
  list->head = list->head->next;
  freeNode(list, toRemove);
  if (list->head)
    list->head->prev = NULL;
  else
    list->tail = NULL;
  list->size--;

  return (1);
}

/**
 * deleteNodeEnd - delete the node at the end of a d_list
 * @list: pointer to list
 *
 * Return: 1 on success, 0 on failure
 */
int deleteNodeEnd(d_list *list) {
  d_node *toRemove;

  if (!list)
    return (0);
  if (!list->size)
    return (0);

  toRemove = list->tail;
  list->tail = list->tail->prev;
  freeNode(list, toRemove);
  if (list->tail)
    list->tail->next = NULL;
  else
    list->head = NULL;
  list->size--;

  return (1);
}

/**
 * deleteNode - delete the node at given index
 * @list: pointer to list
 * @index: the index where the node should be deleted
 *
 * Return: 1 on success, 0 on failure
 */
int deleteNode(d_list *list, unsigned int index) {
  d_node *toRemove;

  if (!list)
    return (0);
  if (index >= list->size)
    return (0);
  if (!index)
    return (deleteNodeStart(list));
  if (index == list->size - 1)
    return (deleteNodeEnd(list));

  toRemove = getNode(list, index);
  toRemove->prev->next = toRemove->next;
  toRemove->next->prev = toRemove->prev;
  freeNode(list, toRemove);
  list->size--;

  return (-1);
}

/**
 * freeNode - free a node
 * @list: pointer to node
 */
void freeNode(d_list *list, d_node *node) {
  if (!node)
    return;

  node->prev = node->next = NULL;
  list->free_func(node->data);
  node->data = NULL;
}
/**
 * freeList - frees all nodes in a d_list
 * @list: pointer to list
 */
void freeList(d_list *list) {
  d_node *ptr, *prv;

  if (!list)
    return;

  ptr = prv = list->head;
  while (ptr) {
    ptr = ptr->next;
    freeNode(list, prv);
    prv = ptr;
  }
  list->head = list->tail = NULL;
  list->size = 0;
}

/**
 * destroyList - destroy a list with all nodes
 * @list: pointer to list
 */
void destroyList(d_list **list) {
  freeList(*list);
  free(*list);
  *list = NULL;
}
