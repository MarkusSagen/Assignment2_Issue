#include "list_linked.h"
#include "hash_table.h"
#include "backend.h"
#include "utils.h"


struct item{
  char *name;
  char *desc;
  int price; 
  ioopm_list_t *shelfs_t;
  int total_amount_in_stock;
  int amount_in_carts;
};
struct cart_merch
{
  char *merch;
  int amount;
};
struct cart
{
  char *id_name;
  ioopm_list_t *items;
};

struct list_out
{
  char **keys;
  int max;
};

struct storage
{
  char *shelf;
  int amount;
};

struct database{
  ioopm_hash_table_t *merch;
  ioopm_hash_table_t *shelf;
  ioopm_hash_table_t *carts;
};


static int get_valid_amount_to_remove(char *question, int maxAmount)
{
  int amount = ask_question_int(question);

  while(amount < 0)
    {
      printf("You can only remove between 0 to %d wares", maxAmount);
      amount = ask_question_int("");
    }
  return amount;
}


static int get_valid_amount(char *question)
{
  int amount = ask_question_int(question);
  while(amount < 0)
    {      
      amount=ask_question_int("You must add a minimum of 1 or 0");
    }
  return amount;
}


static char *get_valid_name (char *question, db_t *db)
{
  char *user_input= ask_question_string(question);
  while(ioopm_hash_table_lookup(db->merch, (elem_t)user_input))
    {
      printf("There already exists a ware with that name. Please specify another\n");
      user_input =ask_question_string(question);
    }
  return user_input;  
}


static int get_valid_int (char *question, int minAmount, int maxAmount)
{
  int user_input = ask_question_int(question);
  while (user_input > maxAmount || user_input < minAmount)
    {
      printf("Incorrect input, choose between 1 and %d", maxAmount);
      user_input = ask_question_int("");
    }
  return user_input;  
}


static char *get_valid_shelf(char* question, db_t *db, char *merch)
{
  char *shelf= ask_question_shelf(question);
  while (ioopm_hash_table_lookup(db->shelf,(elem_t)shelf))
    {
      if ( compare_str_elements(ioopm_hash_table_lookup(db->shelf,(elem_t)shelf),&(elem_t){.string_value_t=merch}))
        {
          return shelf;
        }
      free(shelf);
      puts("This shelf is occupied by another ware. Please specify another\n");
      shelf = ask_question_shelf(question);
    }
  return shelf;
}


static item_t *modify_existing_merch(db_t *db, char *name, bool free_name)
{
  char *shelf = get_valid_shelf("Shelf [A-Z] & [0-99]:", db, name);
  if (ioopm_hash_table_lookup(db->shelf,(elem_t)shelf) &&
      compare_str_elements(ioopm_hash_table_lookup(db->shelf,(elem_t)shelf),&(elem_t){.string_value_t=name}))
    {    
      printf("That ware already existed. How many would you like to add?\n");
      int amount = get_valid_amount("Amount: ");
      item_t *it = lookup_item(db, name);
      ioopm_list_t *list = it->shelfs_t;
      int *amountInStock = find_amount(list,shelf);
      *amountInStock = *amountInStock + amount;
      it->total_amount_in_stock += amount;

      free(shelf);
      return it;              
    }
  else
    {     
      int amount = get_valid_amount("Amount: ");
      storage_t *stock = calloc(1,sizeof(storage_t));
      stock->shelf = shelf;
      stock->amount = amount;
      item_t *item = lookup_item(db,name);
      item->total_amount_in_stock += amount;
      ioopm_list_t *item_list = item->shelfs_t;
      ioopm_linked_list_prepend(item_list,(elem_t){.ptr=stock});
      ioopm_hash_table_insert(db->shelf,(elem_t)shelf,(elem_t)name);

      if (free_name) 
        {
          free(name);
        }
      return NULL;
    }
}


item_t *input_item(db_t *db)
{
  char *name = ask_question_string("Name of ware: ");
  if(ioopm_hash_table_lookup(db->merch, (elem_t){.string_value_t = name}))
    {
      printf("The specified ware already exists. What shelf would you like to add more to?\n");
      return modify_existing_merch(db, name, true);      
    }
  char  *desc = ask_question_string("Description: ");
  int pris    = ask_question_int("Price in öre: ");
  char *shelf = ask_question_shelf("Shelf [A-Z] & [0-99]: ");
  while(ioopm_hash_table_lookup(db->shelf,(elem_t)shelf))
    {
      free(shelf);
      shelf = ask_question_shelf("This shelf is occupied. Please try another shelf number");
    }

  int amount = get_valid_amount("Amount: ");
  item_t *finished_item = make_item(name,desc,pris,shelf,amount);
  
  return finished_item;
}



void insert_item_to_db(db_t *db)
{
  insert(db, input_item(db));  
}




lo_t list_ht(db_t *db)
{
  lo_t result;
  char **keys = array_of_words(db->merch);
  size_t size = db_size(db);

  if (size == 0)
    {
      result.keys=NULL;
      result.max=0;
      free(keys);
      return result;
    }
  else
    {
      sort_keys(keys, size);
      int i= 0;
      int max = 20;

      for (; i < size && i <max; ++i) 
        {         
          printf("%d %s\n", i+1, keys[i]);
        }

      if (i >= size-1)
        {
          printf("There are no more items in the warehouse\n");
          result.keys=keys;
          result.max=max;
          return result;
        }

      char *answer = ask_question_string("[C]ontinue listing or [R]eturn to menu?:");

      while (answer[0] == 'C' || answer[0] =='c')
        {
          max += 20;

          if (i >=size-1)
            {
              printf("There are no more items in the warehouse\n");
              result.keys = keys;
              result.max = max;
              return result;
            }

          for (; i < size && i < max; ++i)
            {
              printf("%d: %s\n", i+1,keys[i]);    
            }

          answer = ask_question_string("[C]ontinue listing or [R]eturn to menu?:");
        }

      result.keys=keys;
      if (max < size)
        {
          result.max = max;
        }
      else
        {
          result.max = size;
        }

      return result;
    }
}



void remove_item_from_db(db_t *db)
{
  lo_t list = list_ht(db);
  char *user_input = list.keys[get_valid_int("Which ware would you like to remove?", 1, list.max)-1];
  
  if (item_is_in_carts(db, user_input))
    {
      puts("You can't remove a ware inside a cart");
    }
  else 
    {
      destroy_merch(db, user_input);    
    }

  free(list.keys);
}


void edit_db(db_t *db)
{
  lo_t list = list_ht(db);  

  if (list.keys != NULL)
    {
      int user_input   = get_valid_int("Which ware would you like to edit?", 1, list.max);  
      item_t *old_item = lookup_item(db, list.keys[user_input-1]);

      char *new_name   = get_valid_name("Name of the ware:", db);
      char *desc       = ask_question_string("Description: ");
      int  pris        = ask_question_int("Price in öre: ");
      item_t *new_item = create_item(new_name, desc, pris, old_item->shelfs_t, old_item->total_amount_in_stock);

      edit_merch_in_db(db, old_item->shelfs_t, new_item, old_item); 
    }

  free(list.keys); 
}



void show_stock(db_t *db)
{
  lo_t list = list_ht(db); 

  if (list.keys != NULL)
    {
      int user_input = get_valid_int("Which ware would you like to display?", 1, list.max);
      item_t *item_to_show = lookup_item(db, list.keys[user_input-1]);
      char **array_of_shelfnames = shelf_array(item_to_show->shelfs_t);
      sort_shelfs(array_of_shelfnames, ioopm_linked_list_size(item_to_show->shelfs_t));

      print_stock(item_to_show, array_of_shelfnames);
      free(array_of_shelfnames);
    }

  free(list.keys);
}


void replenish_item(db_t *db)
{
  lo_t list=list_ht(db); 
  if (list.keys != NULL) 
    {
      int user_input = get_valid_int("Which ware would you like to increase?", 1, list.max);

      char *name_of_merch = list.keys[user_input-1];
      item_t *item_to_replenish = lookup_item(db, name_of_merch);

      char **array_of_shelves= shelf_array(item_to_replenish->shelfs_t);
      sort_keys(array_of_shelves,ioopm_linked_list_size(item_to_replenish->shelfs_t));

      print_stock(item_to_replenish, array_of_shelves);
      printf("The ware is placed on these shelfs. Where would you like to place it else?\n");
      
      modify_existing_merch(db, name_of_merch, false);
      free(array_of_shelves);
    }
  
  free(list.keys);
}


void add_cart(db_t *db)
{
  char *cartName = ask_question_string("Please specify a name on the cart: ");
  while (ioopm_hash_table_lookup(db->carts, (elem_t) cartName))
    {
      free(cartName);
      printf("The given name was already in use. Please specify a new name");
      cartName = ask_question_string("");
    }
  add_cart_to_db(db, cartName);
}


lo_t list_carts(db_t *db)
{
  lo_t result;
  char **keys = array_of_words(db->carts);
  size_t size = ioopm_hash_table_size(db->carts);

  if (size == 0)
    {
      result.keys=NULL;
      free(keys);
      result.max = 0;
      printf("There is no carts in the warehouse\n");
      return result;
    }
  else
    {
      sort_keys(keys, size);
      int i = 0;
      int max = 20;
      for (; i < size && i < max; ++i)
        {
          printf("%d %s\n", i+1,keys[i]);
        }

      if (i >= size-1)
        {
          printf("There is no more carts\n");
          result.keys = keys;
          result.max  = max;
          return  result;
        }

      max+=20;
      char *answer = ask_question_string("[C]ontinue listing or [R]eturn to menu?:");

      while (answer[0] == 'C' || answer[0] =='c')
        {
          if (i >=size-1)
            {
              printf("There is no more carts\n");
              result.keys=keys;
              result.max=max;
              return result;
            }
          for (; i < size && i < max; ++i)
            {
              printf("%d: %s\n", i+1,keys[i]);
            }
          answer = ask_question_string("[C]ontinue listing or return to menu?:");
          max+=20;
        }
      
      result.keys = keys;

      if (max < size)
        {
          result.max = max;
        }
      else
        {
          result.max = size;
        }
      
      return result;
    }
}


void add_to_cart(db_t *db)
{
  lo_t list = list_carts(db);

  if (list.keys != NULL)
    {
      int cartNr = get_valid_int("Which cart would you like to use?\n", 1, list.max); 
      char *cart = list.keys[cartNr-1];
      do 
        { 
          cartNr = get_valid_int("There is no cart with that number, specify another\n", 1, list.max); 
          cart = list.keys[cartNr-1];
        }
      while (cart == NULL);

          elem_t *c  = ioopm_hash_table_lookup(db->carts,(elem_t)cart);
          cart_t *cart_info = c->ptr;
          char *answer;
          lo_t list2 = list_ht(db);
          bool continueListing = true;
   

      do {   
        size_t size2 = ioopm_hash_table_size(db->merch);
        if (size2==0)
          {
            break;
          }
        else
          {
            int max    = (list2.max < size2) ? list2.max : size2;
            int handla = get_valid_int("Which ware would you like to buy?", 1, max);

            elem_t *antal_av_vara = ioopm_hash_table_lookup(db->merch,(elem_t)list2.keys[handla-1]);
            item_t *ptr_vara = (item_t *)antal_av_vara->ptr;
            int total_amount = ptr_vara->total_amount_in_stock - ptr_vara->amount_in_carts;


            if (total_amount == 0)
              {
                printf("There are no wares of that name left\n");
                break;
              }
            else
              {
                printf("There are %d more in stock. How many would you like to add?", total_amount);
                int antal= get_valid_int("", 0, total_amount);

                add_item_to_cart(cart_info, ptr_vara, antal); 
              }
            }
      
          do 
          {
            answer = ask_question_string("[C]ontinue shopping or [R]eturn to menu?:");
            if (answer[0] == 'C' || answer[0] =='c')
              {
                list2 = list_ht(db);      
                break;       
              } 

            if (answer[0] == 'R' || answer[0] =='r')
              {        
                continueListing = false;
                break;
              }
            } 
            while (true);
        }
        while (continueListing == true && cart != NULL);
      
      free(answer);
      free(list.keys);
      free(list2.keys);
    }
}


void remove_cart(db_t *db)
{
  lo_t list = list_carts(db);

  if (list.keys != NULL)
   {
      int vagn = get_valid_int("Which cart would you like to remove?\n", 1, list.max);  
      char *to_remove    = list.keys[vagn-1];
      elem_t *cart_info  = ioopm_hash_table_lookup(db->carts,(elem_t)to_remove);
      empty_cart(db, cart_info->ptr, false);
      ioopm_hash_table_remove(db->carts,(elem_t)to_remove);
    }
  
  free(list.keys);
}



void remove_from_cart(db_t *db)
{
  lo_t list = list_carts(db);

  if (list.keys != 0)
    {
      int cartNr = get_valid_int("Which cart would you like to use?\n", 1, list.max);  ;
      ioopm_list_t *item_list = lookup_cart(db, list.keys[cartNr - 1]);
      size_t list_size = ioopm_linked_list_size(item_list);

      if (list_size == 0)
        {
          printf("Your cart is empty. There are no wares to return\n");
        }
      else
        {
          char **cart_items = merch_array(item_list);
          for (int x = 0; x < list_size; x++)
            { 
              int *amount = find_amount(item_list,cart_items[x]);
              printf("%d. %s, %d\n", x + 1, cart_items[x], *amount);
            }

          int wareNr = get_valid_int("What ware would you like to return?", 1, list.max);
          int amountRemove = get_valid_amount_to_remove("How many would you like to remove?", *find_amount(item_list,cart_items[wareNr-1]));
          item_t *item_being_removed = lookup_item(db, cart_items[wareNr - 1]);
          item_being_removed->amount_in_carts -= amountRemove;
          reduce_amount(item_list, amountRemove, cart_items[wareNr - 1]);

          free(cart_items);
        }
    }
  
  free(list.keys);
}

void checkout(db_t *db)
{
  lo_t list   = list_carts(db);  

  if (list.keys == NULL)
    {
      int cartNr = get_valid_int("Which cart would you like to check out?", 1, list.max);
      elem_t *cart_info = ioopm_hash_table_lookup(db->carts,(elem_t)list.keys[cartNr - 1]);
      cart_t *cart = cart_info->ptr;
      if (ioopm_linked_list_size(cart->items) == 0)
        {
          puts("This cart is empty");
        }
      else
        {
          int cost = calculate_cost(cart,db);
          empty_cart(db, cart, true);
          printf("Your purchase went trough successfully! It cost you a total of %d.%d SEK \n", cost / 100, cost % 100);
        }
      } 

    free(list.keys);
}


void print_merch(db_t *db)
{
  lo_t list = list_ht(db);
  free(list.keys);
}
