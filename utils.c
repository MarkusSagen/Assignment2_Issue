#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"
#include <stdlib.h>
#include <ctype.h>



extern char *strdup(const char *);

/*item_t input_item()
{
 char *name= ask_question_string("Namn på vara: ");
  char  *desc= ask_question_string("Beskrivning: ");
  int pris= ask_question_int("Pris (ören): ");
  char *shelf= ask_question_shelf("Hylla: ");
  int amount = ask_question_int("Amount: ");
  item_t finished_item= make_item(name,desc,pris,shelf,amount);
  return finished_item;
  }*/
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
  answer_t svar= {.int_value=str[0]};
  return svar;
}

char *ask_question_shelf(char *question)
{
  return ask_question(question,is_shelf,(convert_func) strdup).string_value;
}

void print_menu()
{
  printf("[L]ägg till- | [T]a bort- | [R]edigera en vara \nÅn[g]ra senaste ändringen \nLista [h]ela varukatalogen \n[V]isa lager | Öka lag[e]r \n[A]vsluta \nLägg till [k]undvagn | Ta bort kundvag[n] \n[B]örja handla \nRet[u]rnera vara \n[C]hecka ut \n");
}

bool is_menu(char *str)
{
  char c= str[0];
  if (c== '\0')
  {
    return false;
  }
  else if (c== 'c' || c== 'C'||c== 'b'|| c=='B'|| c== 'l' || c== 'L'|| c== 'T'||c== 't'||c== 'R'|| c== 'r'||c== 'G'|| c== 'g'|| c== 'H'||c== 'h'|| c== 'A' || c== 'a'|| c=='V'|| c=='v' || c== 'K' || c=='k' || c=='e'|| c=='E'||c=='n'||c=='N'|| c== 'u'|| c== 'U')
  {
    return true;
  }
  else
  { return false;}

}

char ask_question_menu(char *question)
{
  print_menu();
  answer_t answer= ask_question(question,is_menu,(convert_func) make_char);
  return toupper(answer.char_value);
}

bool is_number(char *str)

{if (isdigit(str[0])=='\0')
{
    return false;
}
    
    
    
else{
    // int x;
    for(int i= 1; i < strlen(str); i++)
    {
        
        
        if (isdigit(str[i])== false)
        {
            return false;
        }
        
        
        /* if (x == strlen(str)-1)
         {
         return true;
         }  */
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
        c =getchar();
        if (c == '\n' && c != EOF)
        {
            buf[acc]= '\0';
            return acc;
            
            /// clean_input_buffer();
            
        }
        
        
        buf[acc]= c;
        acc++;
        if (acc == buf_siz)
        {
            
            /// clear_input_buffer();
            buf[buf_siz -1]= '\0';
            
            return (buf_siz);
            
            ///clear_input_buffer();
            
        }
        
    } while(true);
    
    
    
}


/*char *ask_question_string(char *question, char *buf, int buf_siz)
 {
 
 int length= 0;
 // <-- bufferstorlek 255
 do
 {
 printf("%s\n", question);
 // scanf("%s", buf); // <-- scanf vet ej bufferstorlek! (inläsning av >254 tecken möjligt!)
 length=read_string(buf,buf_siz);
 
 }
 while ( length == 0);
 return buf;
 }*/


char *print(char *ord)
{
    int len= strlen(ord);
    //  char *word=0;
    
    for (int i=0; i < len; i++)
    {
        
        putchar(ord[i]);
        
    }
    return 0;
}


char *println(char *ord)
{
    print(ord);
    putchar('\n');
    return 0;
    
    
}



answer_t ask_question(char *question, check_func check, convert_func convert)
{
    int buf_size= 255;
    char buf[255];
    
    do {
        
        printf("%s \n",question);
        read_string(buf,buf_size);
        
    }while(check(buf)==false);
    
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
    return answer.int_value; // svaret som ett heltal
    
    
    /* int result = 0;
     int conversions = 0;
     do
     {
     printf("%s\n", question);
     conversions = scanf("%d", &result);
     
     clear_input_buffer();
     
     
     }
     while (conversions < 1);
     return result;*/
}

