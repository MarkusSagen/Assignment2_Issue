#include "hash_table.h"

#define Free(ptr) { free(ptr); ptr = NULL;}

struct entry
{
  elem_t key;       // holds the key
  elem_t value;   // holds the value
  entry_t *next; // points to the next entry (possibly NULL) 
};

struct hash_table
{
  int No_Buckets;
  entry_t *buckets;
  hash_fun hash_fun;
  double load_factor; 
  cmp_element_fun key_cmp;
  cmp_element_fun value_cmp;
  size_t sizecount;
};



entry_t* entry_create(elem_t key, elem_t value, entry_t *next)
{
  entry_t *new_entry = calloc(1,sizeof(entry_t));
  new_entry->key = key;
  new_entry->value = value;
  new_entry->next = next;
  return new_entry;
  
}

int count_of_entries(entry_t *entry)
{
  int count = 1;
  entry_t *tmp = entry;
  while(tmp->next != NULL)
  {
    ++count;
    tmp = tmp->next;    
  }
  return count;
}

void entry_destroy(entry_t *previous,entry_t *current)
{
  Free(current);
  previous->next = NULL;
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
  return ht->sizecount;
}

void bucket_destroy(entry_t *current,entry_t *previous,ioopm_hash_table_t *ht)
{
  if (current->next != NULL)
    {
      bucket_destroy(current->next,previous->next,ht);
    }
  entry_destroy(previous,current);
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{  
  for (int i = 0; i < ht->No_Buckets;++i)
    {
      entry_t *current = &ht->buckets[i];
      if (current->next !=NULL)
        {
          bucket_destroy(current->next,current,ht);
        }
    }
  Free(ht->buckets);
  Free(ht);
}

ioopm_hash_table_t *ioopm_hash_table_create(hash_fun hash_fun, cmp_element_fun key_cmp, cmp_element_fun value_cmp)
{
  /// Allocate space for a ioopm_hash_table_t = 17 pointers to
  /// entry_t's, which will be set to NULL
  ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));  result->hash_fun= hash_fun;
  result->key_cmp = key_cmp;
  result->value_cmp = value_cmp;
  result->load_factor = 0.75;
  result->sizecount = 0;
  result->No_Buckets = 17;
  result->buckets = calloc(17, sizeof(entry_t));
  return result;
}


entry_t *find_previous_entry_for_key(ioopm_hash_table_t *ht, elem_t key)
{
  int bucket = ht->hash_fun(&key) % ht->No_Buckets;
  while (bucket < 0)
    {
      bucket = bucket + ht->No_Buckets;
      }
  entry_t *first_entry = &ht->buckets[bucket];
  entry_t *cursor = first_entry;
  entry_t *tmp = cursor;
  if (cursor->next == NULL)
    {
      return cursor;
    }
  while (ht->hash_fun(&cursor->key) < ht->hash_fun(&key)) 
    {
      tmp= cursor;
      if (cursor->next == NULL)
        {
          return tmp;
        }
      else
        {
          cursor = cursor->next;
        }
    }
  return tmp;
}

ioopm_hash_table_t *resize_of_hash_table(ioopm_hash_table_t *ht)
{
  entry_t *old_buckets= ht->buckets;
  size_t size= ioopm_hash_table_size(ht);
  ioopm_list_t *keys= ioopm_hash_table_keys(ht);
  ioopm_list_t*values= ioopm_hash_table_values(ht);
  ioopm_list_iterator_t *iterkey = ioopm_list_iterator(keys);
  ioopm_list_iterator_t *itervalue = ioopm_list_iterator(values);
  elem_t key = *ioopm_iterator_next(iterkey);
  elem_t value = *ioopm_iterator_next(itervalue);
  //link_t *key_link= keys->first->next;
  //link_t *value_link= values->first->next;
  ioopm_hash_table_clear(ht);
  ht->No_Buckets=(ht->No_Buckets*2) +1;
  ht->sizecount = 0;
  ht->buckets=calloc(ht->No_Buckets,sizeof(entry_t));
  for (int i = 0; i < size; i++)
    {
      ioopm_hash_table_insert(ht,key,value);
      key = *ioopm_iterator_next(iterkey);
      value = *ioopm_iterator_next(itervalue);
      //key_link=key_link->next;
      //value_link= value_link->next;
    }
  ioopm_linked_list_destroy(keys);
  ioopm_linked_list_destroy(values);
  ioopm_iterator_destroy(iterkey);
  ioopm_iterator_destroy(itervalue);
  Free(old_buckets);
  return ht;
}
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
  if(ioopm_hash_table_size(ht) >= ht->load_factor *ht->No_Buckets)
    {
      ht= resize_of_hash_table(ht);
    }
  /// Search for an existing entry for a key
  entry_t *entry = find_previous_entry_for_key(ht, key);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if (next != NULL && ht->key_cmp(&next->key, &key))
    {
      next->value = value;
    }
  else
    {
      ht->sizecount += 1;
      entry->next = entry_create(key, value, next);
    }
}

elem_t* ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key)
{
  /// Find the previous entry for key
  entry_t *previous = find_previous_entry_for_key(ht, key);
  entry_t *current = previous->next;
  if (current && ht->hash_fun(&current->key) == ht->hash_fun(&key))
    {
      return &current->value;
    }
  return NULL;
}

elem_t *ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key)
{
 elem_t *result= NULL;
 bool success= ioopm_hash_table_lookup(ht,key);
  if (success)
    {
      entry_t *previous_entry= find_previous_entry_for_key(ht,key);
      entry_t *current= previous_entry->next;
      previous_entry->next= current->next;
      elem_t *tmp= &current->value;
      entry_destroy(previous_entry,current);
      ht->sizecount -= 1;
      return tmp;
    }
  return result;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *h)
{
  return h->sizecount == 0;
}


void ioopm_hash_table_clear(ioopm_hash_table_t *h)
{
  for (int i = 0; i < h->No_Buckets;++i)
    {
      entry_t *current = &h->buckets[i];
      if (current->next !=NULL)
        {
          bucket_destroy(current->next,current,h);
        }
    }
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *h)
{
  ioopm_list_t *list= ioopm_linked_list_create(compare_int_elements);
  size_t size= ioopm_hash_table_size(h);
  elem_t *keys=calloc(size,sizeof(elem_t));
  entry_t *tmp;
  size_t entry_count=0;
  size_t count = 0;
  for (int i = 0; i < h->No_Buckets; i++)
    {
      entry_t *entry= &h->buckets[i];
      if (entry->next != NULL)
        {
          tmp = entry->next;
          count = entry_count+ count_of_entries(tmp);
          for (size_t x= entry_count; x < count; x++)
            {
              *(keys+x) = tmp->key;
              tmp=tmp->next;
            }
          entry_count=count;
        }
    }
  for (size_t i=0; i < count; i++)
    {
      ioopm_linked_list_insert(list,i,keys[i]);     
    }
   Free(keys);
  return list;
}

char** array_of_words(ioopm_hash_table_t *h)
{
  size_t size= ioopm_hash_table_size(h);
  char **words=calloc(size,sizeof(char*));
  entry_t *tmp;
  int entry_count=0;
  int count = 0;
  for (int i = 0; i < h->No_Buckets; i++)
    {
      entry_t *entry= &h->buckets[i];
      if (entry->next != NULL)
        {
          tmp=entry->next;
          count = entry_count + count_of_entries(tmp);
          for (int x= entry_count; x< count; x++)
            {
              *(words+x) = tmp->key.string_value_t;
              tmp=tmp->next;
            }
          entry_count=count;
        }
    }
  return words;
}

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *h)
{
  ioopm_list_t *list= ioopm_linked_list_create(compare_int_elements);
  size_t size= ioopm_hash_table_size(h);
  elem_t *values=calloc(size,sizeof(elem_t));
  entry_t *tmp;
  size_t entry_count=0;
  size_t count = 0;
  for (int i = 0; i < h->No_Buckets; i++)
    {
      entry_t *entry= &h->buckets[i];
      if (entry->next != NULL)
        {
          tmp = entry->next;
          count = entry_count + count_of_entries(tmp);
          for (int x= entry_count; x < count; x++)
            {
              *(values+x) = tmp->value;
              tmp=tmp->next;
            }
          entry_count = count;
        }
    }
  for (int i=0; i < count; i++)
    {
      ioopm_linked_list_insert(list,i,values[i]);
    }
   Free(values);
  return list;

  
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
  size_t size = ioopm_hash_table_size(ht);
  bool result;
  ioopm_list_t *keys = ioopm_hash_table_keys(ht);
  ioopm_list_iterator_t *iterkey = ioopm_list_iterator(keys);
  elem_t k = *ioopm_iterator_next(iterkey);
    for (int i = 0;(size_t) i < size; ++i)
      {
	result = ht->key_cmp(&k, &key);
	if (result)
	  {
	    ioopm_linked_list_destroy(keys);
	    ioopm_iterator_destroy(iterkey);
	    return result;
	  }
	k = *ioopm_iterator_next(iterkey);
      }
    ioopm_linked_list_destroy(keys);
    ioopm_iterator_destroy(iterkey);
    return result;
}


bool ioopm_hash_table_any(ioopm_hash_table_t *h, ioopm_predicate pred, void *arg)
{
  size_t size = ioopm_hash_table_size(h);
  ioopm_list_t *keys = ioopm_hash_table_keys(h);
  ioopm_list_t *values = ioopm_hash_table_values(h);
  ioopm_list_iterator_t *iterkey = ioopm_list_iterator(keys);
  ioopm_list_iterator_t *itervalue = ioopm_list_iterator(values);
  elem_t key = *ioopm_iterator_next(iterkey);
  elem_t value = *ioopm_iterator_next(itervalue);
  for (int i = 0;  i < size; ++i)
    { bool result;
      result = pred(key,value, (elem_t)arg, NULL);
    if (result)
      {
      ioopm_linked_list_destroy(keys);
      ioopm_linked_list_destroy(values);
      ioopm_iterator_destroy(iterkey);
      ioopm_iterator_destroy(itervalue);
      return true;
      }
    key = *ioopm_iterator_next(iterkey);
    value = *ioopm_iterator_next(itervalue);
    }
  ioopm_linked_list_destroy(keys);
  ioopm_linked_list_destroy(values);
  ioopm_iterator_destroy(iterkey);
  ioopm_iterator_destroy(itervalue);
  return false;  
}


bool ioopm_hash_table_all(ioopm_hash_table_t *h, ioopm_predicate pred, void *arg)
{
  size_t size = ioopm_hash_table_size(h);
  ioopm_list_t *keys = ioopm_hash_table_keys(h);
  ioopm_list_t *values = ioopm_hash_table_values(h);
  bool result = true;
  ioopm_list_iterator_t *iterkey = ioopm_list_iterator(keys);
  ioopm_list_iterator_t *itervalue = ioopm_list_iterator(values);
  elem_t key = *ioopm_iterator_next(iterkey);
  elem_t value = *ioopm_iterator_next(itervalue);
  for (int i = 0; i < size && result; ++i)
    {
      result = result && pred(key, value, (elem_t)arg, NULL);
      key = *ioopm_iterator_next(iterkey);
      value = *ioopm_iterator_next(itervalue);
    }    
  ioopm_linked_list_destroy(keys);
  ioopm_linked_list_destroy(values);
  ioopm_iterator_destroy(iterkey);
  ioopm_iterator_destroy(itervalue);
 
  return result; 
}  
bool has_value(elem_t key,elem_t value, void *arg)
{
  char **value2_ptr =arg;
  char *value2 = *value2_ptr;
  if (strcmp(value.string_value_t,value2)==0)
    return true;
  return false;
  }

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
  size_t size = ioopm_hash_table_size(ht);
  bool result;
  ioopm_list_t *values = ioopm_hash_table_values(ht);
  ioopm_list_iterator_t *itervalue = ioopm_list_iterator(values);
  elem_t v = *ioopm_iterator_next(itervalue);

  for (int i = 0;(size_t) i < size; ++i)
    {
      result = ht->value_cmp(&v, &value);
      if (result)
	{
	  ioopm_linked_list_destroy(values);
	  ioopm_iterator_destroy(itervalue);

	  return result;
	}
      v = *ioopm_iterator_next(itervalue);

    }
  ioopm_linked_list_destroy(values);
  ioopm_iterator_destroy(itervalue);
  return result;
}
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *h, ioopm_apply_function apply_fun , void *arg)
{
  size_t size = ioopm_hash_table_size(h);
  ioopm_list_t *keys = ioopm_hash_table_keys(h);
  ioopm_list_t *values = ioopm_hash_table_values(h);
  ioopm_list_iterator_t *iterkey = ioopm_list_iterator(keys);
  ioopm_list_iterator_t *itervalue = ioopm_list_iterator(values);
  elem_t key = *ioopm_iterator_next(iterkey);
  elem_t value = *ioopm_iterator_next(itervalue);
  for (int i = 0;(size_t) i < size; ++i)
    {
       apply_fun(key,value, arg);
       key = *ioopm_iterator_next(iterkey);
       value = *ioopm_iterator_next(itervalue);
    }
  ioopm_linked_list_destroy(keys);
  ioopm_linked_list_destroy(values);
  ioopm_iterator_destroy(iterkey);
  ioopm_iterator_destroy(itervalue);
}
int int_to_hash(elem_t *elem)
{
  return elem->int_value_t;
}

int string_to_hash(elem_t *elem)
{
  int result = 0;
  char *str= elem->string_value_t;
  if (str == NULL)
    {
      return result;
    }
  do
    {
      result = result * 31 + *str;
    }
  while (*++str != '\0'); 
  if (result <0)
    {
      result *=-1;
    }
  return result;
}



//TESTFUNKTIONER, BOKSTAVLIGEN BARA ANVÄND I TESTER



bool replace_string_value(elem_t key,elem_t value, void *arg)
{
  ioopm_hash_table_t *ht = arg;
  entry_t *previous_entry= find_previous_entry_for_key(ht,key);
  entry_t *spected_entry= previous_entry->next;
  spected_entry->key = key;
  spected_entry->value = (elem_t)"hej";
  return true;
 }

bool matching_keys_and_values(elem_t key,elem_t value, void *arg)
{
  ioopm_hash_table_t *ht = arg;
  entry_t *previous_entry= find_previous_entry_for_key(ht,key);
  entry_t *spected_entry= previous_entry->next;
  return (ht->key_cmp(&spected_entry->key, &key) && ht->value_cmp(&spected_entry->value, &value));
}

bool do_the_key_and_value_have_the_same_insert(elem_t key,elem_t value, void *arg)
{
  ioopm_hash_table_t *ht = arg;
  return ht->key_cmp(&key,&value);
}

