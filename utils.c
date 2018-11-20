#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"
#include <stdlib.h>
#include <ctype.h>




extern char *strdup(const char *);


bool is_shelf(char *str)
{
  if((str[0] >= 65 && str[0]<=90) && strlen(str)>1&& strlen(str) <4)
    {
      for(int i= 1; i < strlen(str); i++)
        {
          if (isdigit(str[i])== false)
            {
              return false;
            }
        }
      return true;
    }

  return false;
}


answer_t make_char(char *str)
{
  answer_t answer= {.int_value=str[0]};
  return answer;
}


char *ask_question_shelf(char *question)
{
  return ask_question(question,is_shelf,(convert_func) strdup).string_value;
}


// List of the main menu
void print_menu()
{
    printf("\n");

    printf("[A]dd ware \n");
    printf("[R]emove ware \n");
    printf("[E]dit ware \n");
    printf("[L]ist cart \n");
    printf("[S]how stock \n");
    printf("[I]ncrease stock \n");
    printf("[M]ake new cart \n");
    printf("[D]elete cart \n");
    printf("[B]egin shopping \n");
    printf("[P]lace back ware from cart \n");
    printf("[C]heckout \n");
    printf("[U]ndo last command \n");
    printf("[Q]uit \n");
    
    printf("\n");

}


bool is_menu(char *str)
{
  char val = str[0];

  if (val == '\0')
    {
      return false;
    }
  // BAD!!
  else if ( val == 'a' || val == 'A'|| val == 'b'|| val =='B'|| val == 'c' || val == 'C'|| val == 'd'|| val == 'D'|| val == 'e'|| val == 'E'|| val == 'i'|| val == 'I'|| val == 'l'|| val == 'L'|| val == 'm' || val == 'M'|| val =='p'|| val =='P' || val == 'q' || val =='Q' || val =='r'|| val =='R'|| val =='s'|| val =='S'|| val == 'u'|| val == 'U')
    {
      return true;
    }
  else
    { 
      return false;
    }
}


char ask_question_menu(char *question)
{
  print_menu();
  answer_t answer= ask_question(question,is_menu,(convert_func) make_char);
  return toupper(answer.char_value);
}

bool is_number(char *str)
{
  if (isdigit(str[0])=='\0')
    {
      return false;
    }
  else
    {
      for(int i= 1; i < strlen(str); i++)
      {   
        if (isdigit(str[i])== false)
        {
          return false;
        }
      }

      return true;
    }
}

bool not_empty(char *str)
{
  return strlen(str) > 0;
}


void clear_input_buffer()
{
    int c;

    do
    {
      c = getchar();
    }
    while (c != '\n' && c != EOF);
}


int read_string(char *buf, int buf_siz)
{
    char c=0;
    int acc= 0;

    do
    {
      c = getchar();
      if (c == '\n' && c != EOF)
      {
        buf[acc]= '\0';
        return acc;  
      }
      
      buf[acc]= c;
      acc++;

      if (acc == buf_siz)
      {
        buf[buf_siz -1]= '\0';
          
        return (buf_siz); 
      }
    } 
    while(true);
}



char *print(char *word)
{
    int len= strlen(word);
   
    for (int i=0; i < len; i++)
    {
   
      putchar(word[i]);
    }

    return 0;
}


char *println(char *word)
{
    print(word);
    putchar('\n');

    return 0; 
}



answer_t ask_question(char *question, check_func check, convert_func convert)
{
    int buf_size= 255;
    char buf[255];
    
    do 
    {
      printf("%s \n",question);
      read_string(buf,buf_size);  
    }
    while(check(buf)==false);
    
    return convert(buf);
}


char *convert(char *input)
{
  int length =  strlen(input);
  input[0] = toupper(input[0]);
  for (int i = 1; i < length; ++i)
    {
      input[i] = tolower(input[i]);      
    }
  return input;
}


char *ask_question_string(char *question)
{
  return convert(ask_question(question, not_empty, (convert_func) strdup).string_value);
}


int ask_question_int(char *question)
{
    answer_t answer = ask_question(question, is_number, (convert_func) atoi);
    return answer.int_value; 
    
}

