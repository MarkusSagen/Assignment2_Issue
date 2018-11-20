#include "utils.h"
#include "backend.h"
#include "frontend.h"
#include <stdio.h>

void event_loop(db_t *db)
{
  int answer = 0;
  

  do {
      answer = ask_question_menu("What would you like to do?");

      switch(answer) {
        case 'A' :    // Add ware
          insert_item_to_db(db);
          break;
        case 'R' :    // Remove ware
          remove_item_from_db(db);
          break;
        case 'E' :    // Edit ware   
          edit_db(db);
          break;
        case 'L' :    // List ware
          print_merch(db);
          break;
        case 'S' :    // Show stock 
          show_stock(db);
          break;
        case 'I' :    // Increase stock
          replenish_item(db);
          break;
        case 'M' :    // Make new cart
          add_cart(db);
          break;
        case 'D' :    // Delete cart
          remove_cart(db);
          break;
        case 'B' :    // Begin shopping
          add_to_cart(db);
          break;
        case 'P' :    // Place back ware from cart
          remove_from_cart(db);
          break;
        case 'C' :    // Checkout
          checkout(db);   
          break;
        case 'U' :    // Undo
          printf("Not yet implemented\n");
          break;
        case 'Q' :    // Quit
          break;
        default: 
          ;
      }
      
    } while (answer != 'Q');
      destroy_db(db);
}

int main()
{
  db_t *db = create_db();
  event_loop(db);
}


