#include "list_linked.h"

#include <stdlib.h>

struct list
{
  elem_t value;
  int size;
  ioopm_list_t *next;
  ioopm_list_t *last;
  ioopm_eq_function fun;
};


ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function fun)
{
  ioopm_list_t *list = calloc(1, sizeof(ioopm_list_t));
  list->fun = fun;
  return list;
}

void ioopm_linked_list_destroy(ioopm_list_t *list) {
  ioopm_list_t *tmp = list->next;
  free(list);
  if (tmp != NULL) ioopm_linked_list_destroy(tmp);
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value)
{
  ioopm_list_t *tmp = ioopm_linked_list_create(NULL);
  tmp->value = value;
  tmp->next = list->next; 
  list->next = tmp;
  list->size += 1;
}

void ioopm_linked_list_append(ioopm_list_t *list, elem_t value) {
  ioopm_list_t *tmp = ioopm_linked_list_create(NULL);
  tmp->value = value;
  if (list->last == NULL) { //La till basfall för den tomma listan, fick segfault i testerna annars.
    list->last = tmp;
    list->next = tmp;
  }
  else {
    list->last->next = tmp;
    list->last = tmp;
  }
  list->size += 1;
}

void ioopm_linked_list_insert(ioopm_list_t *list, int index, elem_t value)
{
  ioopm_list_t *insert = ioopm_linked_list_create(NULL);
  insert->value = value;
  ioopm_list_t *prev = list;
  if (list->next == NULL) {
    list->next = insert;
    list->last = insert;
  }
  else {
    for (int i = 0; i < index; ++i)
      {
        prev = prev->next;
      }
    insert->next = prev->next;
    if (prev->next == NULL) list->last = insert;
    prev->next = insert;
  }
  list->size += 1;
}

elem_t *ioopm_linked_list_get(ioopm_list_t *list, int index)
{
  ioopm_list_t *tmp = list->next;
  for (int i = 0; i < index; ++i)
    {
    if (tmp == NULL) return NULL;
      tmp = tmp->next;
    }
  return &(tmp->value);
}


/*
int *ioopm_linked_list_remove(ioopm_list_t *list, int index) {
  ioopm_list_t *tmp = NULL;
  ioopm_list_t *tmp_prev;
  int *tmp_value = calloc(1, sizeof(int));
  int i=0;
  while (true) {
    tmp_prev = tmp;
    tmp = list->next;
    if (tmp == NULL) return NULL;
    if (i==index) {
      tmp_prev->next = tmp->next;
      tmp_value = &(tmp->value);
      free(tmp);
      list->size -= 1;
      return tmp_value;
    }
    ++i;
  }
}
*/

elem_t ioopm_linked_list_remove(ioopm_list_t *list, int index) {
  ioopm_list_t **previous = &list;
  while (index > 0) {
    previous = &(*previous)->next;
    index -= 1;
  }
  elem_t removed = (*previous)->next->value;
  ioopm_list_t *tmp = (*previous)->next;
  (*previous)->next = (*previous)->next->next;
  free(tmp);
  list->size -= 1;
  if ((*previous)->next == NULL) list->last = *previous;
  return removed;
}


bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t element)
{
  ioopm_list_t **test = &list;
  while ((*test)->next != NULL) {
    if (list->fun(&((*test)->next->value), &element)) return true;
    test = &((*test)->next);
  }
  return false;
}


int ioopm_linked_list_size(ioopm_list_t *list)
{
  return list->size;  
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list) {
  if (list->next == NULL) return true;
  else return false;
}


void ioopm_linked_list_clear(ioopm_list_t *list) {
  ioopm_list_t *tmp = list->next;
  ioopm_list_t *tmp_rm;
  list->next = NULL;
  bool loop = (tmp != NULL);
  while (loop) {
  tmp_rm = tmp;
  tmp = tmp->next;
  free(tmp_rm);
  if (tmp == NULL) return;
  }
}


bool ioopm_linked_list_all(ioopm_list_t *list, bool (*prop)(elem_t, elem_t), void *x)
{
  ioopm_list_t **test = &(list->next);
  while (*test != NULL) {
    if (!prop((*test)->value, *(elem_t *)x)) return false;
    test = &((*test)->next); 
  }
  return true;  
}

bool ioopm_linked_list_any(ioopm_list_t *list, bool (*prop)(elem_t, elem_t), void *x)
{
  ioopm_list_t **ptr_to_list = &(list->next);
  while (*ptr_to_list != NULL) {
    if (prop((*ptr_to_list)->value, *(elem_t *)x)) return true;
    ptr_to_list = &((*ptr_to_list)->next); 
  }
  return false;   
}

void ioopm_linked_apply_to_all(ioopm_list_t *list, void (*fun)(elem_t *, elem_t *), void *x)
{
  ioopm_list_t **ptr_to_list = &(list->next);
  while (*ptr_to_list != NULL) {
    fun(&(*ptr_to_list)->value, (elem_t *)x);
    ptr_to_list = &((*ptr_to_list)->next); 
  }  
}
