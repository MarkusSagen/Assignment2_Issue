#include "list_linked.h"
#include "iterator.h"
#include <stdlib.h>


struct list
{
  elem_t value;
  int size;
  ioopm_list_t *next;
  ioopm_list_t *last;  
};

struct ioopm_list_iterator
{
  ioopm_list_t *current;
  ioopm_list_t *dummy;
};

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list)
{
  ioopm_list_iterator_t *iterator = calloc(1, sizeof(ioopm_list_iterator_t));
  iterator->current = list;
  iterator->dummy = list;
                         
  return iterator; 
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
  if (iter->current->next == NULL) return false;
  return iter->current->next->next;
}

elem_t *ioopm_iterator_next(ioopm_list_iterator_t *iter)
{  
  if (iter->current->next->next == NULL) return NULL; 
  iter->current = iter->current->next; 
  return &iter->current->value;
}

elem_t ioopm_iterator_remove(ioopm_list_iterator_t *iter) {
  ioopm_list_t *rm = iter->current->next;
  elem_t ret = iter->current->next->value;
  iter->current->next = iter->current->next->next;
  free(rm);
  iter->dummy->size += 1;
  if (!iter->current->next) iter->dummy->last = iter->current;
  return ret;
}

void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t element)
{
  ioopm_list_t *insert = ioopm_linked_list_create(NULL);
  insert->value = element;
  insert->next = iter->current->next;
  if (!ioopm_iterator_has_next(iter)) {
    iter->current->next = insert;
    iter->dummy->last = insert;
  }
  else iter->current->next = insert;
  iter->dummy->size += 1;
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
  iter->current = iter->dummy;  
}


elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter) 
{
  return iter->current->next->value; 
}


void ioopm_iterator_destroy(ioopm_list_iterator_t *iter)
{
  iter->current = NULL;
  iter->dummy = NULL;
  free(iter);  
}
