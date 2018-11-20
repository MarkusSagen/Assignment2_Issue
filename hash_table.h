#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "list_linked.h"

#ifndef __ASSIGNMENT1_H__
#define __ASSIGNMENT1_H__

typedef struct entry entry_t;

typedef struct hash_table ioopm_hash_table_t;
typedef void(*ioopm_apply_function)(elem_t key, elem_t value, void *extra);
typedef int(*hash_fun)(elem_t *elem);


///@brief makes the hashtable bigger when the number of entries gets too big
///@param ht the hashtable to be resized (not empty)
ioopm_hash_table_t *resize_of_hash_table(ioopm_hash_table_t *ht);

///@brief returns the number of entries in the hashtable
///@ ht the hashtable to count entries
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

///@brief create the hashtable
///@param fun hashfunction for keys
///@param cmp function to compare keys
///@param comp function to compare values
ioopm_hash_table_t *ioopm_hash_table_create(hash_fun fun, cmp_element_fun cmp, cmp_element_fun comp);

///@brief insert value with specified key
///@param ht hashtable where you insert value and key
///@param key key of type elem_t
///@param value value of type elem_t
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

///@brief lookup if key match up to a value in the hashtable if so return the adress of the value, otherwise returns NULL
///@param ht hashtable
///@param key key of type elem_t
elem_t* ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key);

///@brief removes element from the hashtable that matches to the key and returns element removed. If key is not found, returns NULL and no element is removed.
///@param ht hashtable
///@param key key is of type elem_t
elem_t *ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key);

///@brief destroys the hashtable, freeing all entries, buckets and the hashtable itself.
///@param ht, the hashtable to be destroyed
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

///@brief checks if the hashtable has no entries. If empty returns true, otherwise false.
///@param ht the hashtable to be checked
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

///@brief removes and frees all entries from the hashtable 
///@param ht hashtable to be cleared
void ioopm_hash_table_clear(ioopm_hash_table_t *ht);

///@brief makes a list of all the keys in the hashtable
///@param ht the hashtable
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht);

///@brief makes a list of all the values in the hashtable
///@param ht the hashtable
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht);

///@brief checks if the hashtable has a specific key. If key is found, returns true and otherwise false
///@param ht hashtable to be checked
///@param key expected key to be in the hashtable
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

///@brief checks if the hashtable has a specific value. If value is found, returns true and otherwise false
///@param ht hashtable to be checked
///@param value expected value to be in the hashtable
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value);

///@brief checks if predicate holds true for any of the entries
///@param ht hashtable to be checked
///@param pred a function that checks if any elements hold for the predicate
///@param *arg extra argument to be used if needed for pred
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);


///@brief checks if predicate holds true for all of the entries
///@param ht hashtable to be checked
///@param pred a function that checks if all elements hold for the predicate
///@param *arg extra argument to be used if needed for pred
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg);

///@brief applies a function on all entries in the hashtable
///@param ht hashtable
///@param apply_fun the function to be applied to all elements
///@param *arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *h, ioopm_apply_function apply_fun, void *arg);


///FOR TEST ONLY, NEED FOR HASH_TABLE_TESTS

bool replace_string_value(elem_t key,elem_t value, void *arg);
int int_to_hash(elem_t *elem);
int string_to_hash(elem_t *elem);
char** array_of_words(ioopm_hash_table_t *h);
bool do_the_key_and_value_have_the_same_insert(elem_t key,elem_t value, void *arg);
bool has_value(elem_t key,elem_t str, void *arg);
bool matching_keys_and_values(elem_t key,elem_t value, void *arg);




#endif
