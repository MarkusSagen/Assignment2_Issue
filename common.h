#pragma once
#include <stdbool.h>
#include <stdlib.h>


typedef union elem elem_t;
union elem
{
  /// TODO: update the names of the fields to something better?                                                                                                                                             
  int int_value_t;
  unsigned int unsigned_int_t;
  bool bool_value_t;
  float float_value_t;
  void *ptr;
  char char_value_t;
  char *string_value_t;
 
  /// other choices certainly possible
};


typedef bool(*cmp_element_fun)(elem_t *a, elem_t *b);
bool compare_int_elements(elem_t *a, elem_t *b);
bool compare_bool_elements(elem_t *a, elem_t *b);
bool compare_float_elements(elem_t *a, elem_t *b);
bool compare_unint_elements(elem_t *a, elem_t *b);
bool compare_void_elements(elem_t *a, elem_t *b);
bool compare_str_elements(elem_t *a, elem_t *b);

typedef union elem elem_t;

typedef bool(*ioopm_eq_function)(elem_t *a, elem_t *b);
typedef bool(*ioopm_predicate)(elem_t key, elem_t value, elem_t extra, ioopm_eq_function fun);








