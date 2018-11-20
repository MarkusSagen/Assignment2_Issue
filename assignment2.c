#include "utils.h"
#include "backend.h"
#include "frontend.h"
#include <stdio.h>

void event_loop(db_t *db)
{
  int answer = 0;
  do {
    
    answer = ask_question_menu("Vad vill du göra?");
    if (answer=='L')
      {
        insert_item_to_db(db);

      }
    else if(answer=='T')
      {
        remove_item_from_db(db);

      }
    else if (answer=='R')
      {
        edit_db(db);

      }
    else if (answer== 'G')
      {
        printf("Not yet implemented\n");

      }
    else if (answer=='H')
      {
        print_merch(db);

      }
    else if (answer== 'V')
      {
        show_stock(db);

      }
    else if (answer == 'E')
      {
        replenish_item(db);

      }
    else if(answer== 'K')
      {
        add_cart(db);
      
      }
    else if(answer== 'N')
      {
        remove_cart(db);
      
      }
    else if(answer =='B')
      {
        add_to_cart(db);
      
      }
    else if (answer == 'U')
      {
        remove_from_cart(db);
      
      }
    else if (answer== 'C')
      {
        checkout(db);     
      }
  } while (answer != 'A');
   destroy_db(db);
}

int main()
{
  db_t *db = create_db();
  event_loop(db);
  /* insert_item_to_db(db);
  insert_item_to_db(db);
  insert_item_to_db(db);
  add_cart(db);
  add_to_cart(db);
  add_to_cart(db);
  remove_from_cart(db);
  */
}
