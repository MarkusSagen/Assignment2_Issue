#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdbool.h>

typedef union { // Not: undvik gärna så här korta namn på variabler! Varför?
  int   int_value;
  float float_value;
  char *string_value;
  char  char_value;
} answer_t;




typedef bool(*check_func)(char *str);


typedef answer_t(*convert_func) (char *str);
int read_string(char *buf, int buf_siz);
bool is_number(char *str);
int ask_question_int(char *question);
char *ask_question_string(char *question);
char *print(char *ord);
char *println(char *ord);
extern char *strdup(const char *);
bool not_empty(char *str);
answer_t ask_question(char *question, check_func check, convert_func convert);
bool is_shelf(char *str);
answer_t make_char(char *str);
char *ask_question_shelf(char *question);
void print_menu();
bool is_menu(char *str);
char ask_question_menu(char *question);
#endif
