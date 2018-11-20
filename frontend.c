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

static int get_valid_amount_to_remove(char *question, int max)
{
  int amount = ask_question_int(question);
  while(amount <= 0)
    {
      printf("Du kan bara ta bort 0 till %d varor", max);
      amount = ask_question_int("");
    }
  return amount;
}

static int get_valid_amount(char *question)
{
  int amount = ask_question_int(question);
  while(amount <=0)
    {      
      amount=ask_question_int("Du måste minst lägga in 1");
    }
  return amount;
}

static char *get_valid_name (char *question, db_t *db)
{
  char *user_input= ask_question_string(question);
  while(ioopm_hash_table_lookup(db->merch, (elem_t)user_input))
    {
      printf("Det finns redan en vara med det namnet i varuhuset, ta något annat\n");
      user_input =ask_question_string(question);
    }
  return user_input;  
}

static int get_valid_int (char *question, int min, int max)
{
  int user_input = ask_question_int(question);
  while (user_input > max || user_input < min)
    {
      printf("Wrong input, choose between 1 and %d", max);
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
      puts("Den hyllan är upptagen av en annan vara, försök igen\n");
      shelf = ask_question_shelf(question);
    }
  return shelf;
}

static item_t *modify_existing_merch(db_t *db, char *name, bool free_name)
{
  char *shelf = get_valid_shelf("Hylla [A-Z] & [0-99]:", db, name);
  if (ioopm_hash_table_lookup(db->shelf,(elem_t)shelf) &&
      compare_str_elements(ioopm_hash_table_lookup(db->shelf,(elem_t)shelf),&(elem_t){.string_value_t=name}))
    {    
      printf("Din vara fanns här redan, hur många vill du lägga till?\n");
      int amount = get_valid_amount("Amount: ");
      item_t *it = lookup_item(db, name);
      ioopm_list_t *list=it->shelfs_t;
      int *x=find_amount(list,shelf);
      *x= *x+amount;
      it->total_amount_in_stock+=amount;

      free(shelf);
      return it;              
    }
  else
    {     
      int amount = get_valid_amount("Amount: ");
      storage_t *stock = calloc(1,sizeof(storage_t));
      stock->shelf=shelf;
      stock->amount=amount;
      item_t *item = lookup_item(db,name);
      item->total_amount_in_stock+=amount;
      ioopm_list_t *item_list=item->shelfs_t;
      ioopm_linked_list_prepend(item_list,(elem_t){.ptr=stock});
      ioopm_hash_table_insert(db->shelf,(elem_t)shelf,(elem_t)name);

      if (free_name) free(name);
      return NULL;
    }
}

item_t *input_item(db_t *db)
{
  char *name = ask_question_string("Namn på vara: ");
  if(ioopm_hash_table_lookup(db->merch, (elem_t){.string_value_t=name}))
    {
      printf("Varan finns redan, vilken hylla vill du fylla på med mer?\n");
      return modify_existing_merch(db, name, true);      
    }
  char  *desc = ask_question_string("Beskrivning: ");
  int pris    = ask_question_int("Pris (ören): ");
  char *shelf = ask_question_shelf("Hylla [A-Z] & [0-99]: ");
  while(ioopm_hash_table_lookup(db->shelf,(elem_t)shelf))
    {
      free(shelf);
      shelf = ask_question_shelf("Hyllan är upptagen av en annan vara, försök igen");
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
  if (size==0)
    {
      result.keys=NULL;
      result.max=0;
      free(keys);
      return result;
    }
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
      return  result;
    }

  char *answer = ask_question_string("[C]ontinue listing or [R]eturn to menu?:");

  while (answer[0] == 'C' || answer[0] =='c')
    {
      max += 20;
      if (i >=size-1)
        {
          printf("There are no more items in the warehouse\n");
          result.keys=keys;
          result.max=max;
          return result;
        }
      for (; i < size && i < max; ++i)
        {
          printf("%d: %s\n", i+1,keys[i]);    
        }
      answer = ask_question_string("[C]ontinue listing or [R]eturn to menu?:");
    }
  
  result.keys=keys;
  result.max = (max < size) ? max : size;
  return result;
}



void remove_item_from_db(db_t *db)
{
  lo_t list        = list_ht(db);
  char *user_input = list.keys[get_valid_int("Vilken vara vill du radera?", 1, list.max)-1];
  if (item_is_in_carts(db, user_input))
    {
      puts("Du kan inte ta bort en vara som ligger i en kundvagn");
      free(list.keys);
      return;
    }
  destroy_merch(db, user_input);    
  free(list.keys);
}

void edit_db(db_t *db)
{
  lo_t list        = list_ht(db);  
  if (list.keys == NULL)
    {
      free(list.keys);
      return;    
    }
  int user_input   = get_valid_int("Vilken vara vill du redigera?", 1, list.max);  
  item_t *old_item = lookup_item(db, list.keys[user_input-1]);

  char *new_name   = get_valid_name("Namn på varan?:", db);
  char *desc       = ask_question_string("Beskrivning: ");
  int  pris        = ask_question_int("Pris (ören): ");
  item_t *new_item = create_item(new_name, desc, pris, old_item->shelfs_t, old_item->total_amount_in_stock);

  edit_merch_in_db(db, old_item->shelfs_t, new_item, old_item);  
  free(list.keys);
}



void show_stock(db_t *db)
{
  lo_t list = list_ht(db); 
  if (list.keys == NULL)
    {
      free(list.keys);
      return;    
    }
  int user_input = get_valid_int("Vilken vara vill du visa?", 1, list.max);
  
  item_t *item_to_show = lookup_item(db, list.keys[user_input-1]);
  char **array_of_shelfnames = shelf_array(item_to_show->shelfs_t);
  sort_shelfs(array_of_shelfnames, ioopm_linked_list_size(item_to_show->shelfs_t));
  
  print_stock(item_to_show, array_of_shelfnames);
  free(array_of_shelfnames);
  free(list.keys);
}


void replenish_item(db_t *db)
{
  lo_t list=list_ht(db); 
  if (list.keys == NULL) {
    free(list.keys);
    return;    
  }
  int user_input = get_valid_int("Vilken vara vill du öka lagret på?", 1, list.max);

  char *name_of_merch = list.keys[user_input-1];
  item_t *item_to_replenish = lookup_item(db, name_of_merch);
  char **array_of_shelves= shelf_array(item_to_replenish->shelfs_t);
  sort_keys(array_of_shelves,ioopm_linked_list_size(item_to_replenish->shelfs_t));

  print_stock(item_to_replenish, array_of_shelves);
  printf("Varan finns på dessa hyllor, var vill du lägga till fler?\n");
  

  modify_existing_merch(db, name_of_merch, false);

  free(array_of_shelves);
  free(list.keys);
}

void add_cart(db_t *db)
{
  char *id = ask_question_string("Var snäll och ge din kundvagn ett namn");
  while (ioopm_hash_table_lookup(db->carts,(elem_t)id))
    {
      free(id);
      printf("Namnet är tyvärr upptaget, var snäll och skriv in ett nytt namn");
      id = ask_question_string(" ");
    }
  add_cart_to_db(db, id);
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
      result.max=0;
      printf("There is no carts in the warehouse\n");
      return result;
    }

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
    {if (i >=size-1)
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
  result.max  = (max < size) ? max : size;
  return result;
}



void remove_cart(db_t *db)
{
  lo_t list = list_carts(db);
  if (list.keys == NULL)
    {
      free(list.keys);
      return;
    }
  int vagn = get_valid_int("Vilken vagn vill du ta bort?\n", 1, list.max);  
  char *to_remove    = list.keys[vagn-1];
  elem_t *cart_info  = ioopm_hash_table_lookup(db->carts,(elem_t)to_remove);
  empty_cart(db, cart_info->ptr, false);
  ioopm_hash_table_remove(db->carts,(elem_t)to_remove);
  free(list.keys);
}


void add_to_cart(db_t *db)
{
  lo_t list = list_carts(db);
  if (list.keys == NULL)
    {
      free(list.keys);
      return;
    }
  int vara = get_valid_int("Vilken vagn vill du använda?\n", 1, list.max);  
  char *cart = list.keys[vara-1];
  elem_t *c  = ioopm_hash_table_lookup(db->carts,(elem_t)cart);
  cart_t *cart_info = c->ptr;
  char *answer;
  int x = 1;
  lo_t list2 = list_ht(db);
  do {   
    size_t size2 = ioopm_hash_table_size(db->merch);
    if(size2==0)
      {
        free(list.keys);
        free(list2.keys);
        return;
      }
    int max    = (list2.max < size2) ? list2.max : size2;
    int handla = get_valid_int("Vilken vara vill du handla?", 1, max);

    
    elem_t *antal_av_vara = ioopm_hash_table_lookup(db->merch,(elem_t)list2.keys[handla-1]);
    item_t *ptr_vara = (item_t *)antal_av_vara->ptr;
    int total_amount = ptr_vara->total_amount_in_stock - ptr_vara->amount_in_carts;
    if (total_amount == 0)
      {
        printf("Finns inga %s kvar på våra hyllor,de ligger i någons kundvagn\n",list2.keys[handla-1]);
        free(list.keys);
        free(list2.keys);
        return;
      }
    printf("Det finns %d i lagret, hur många vill du ha", total_amount);
    int antal= get_valid_int("", 0, total_amount);

    add_item_to_cart(cart_info, ptr_vara, antal);
    
    do {
      answer = ask_question_string("[C]ontinue shopping or [R]eturn to menu?:");
      if (answer[0] == 'C' || answer[0] =='c')
        {
          free(list2.keys);
          list2 = list_ht(db);
          x = 1;
          free(answer);        
          break;       
        }        
      if (answer[0] == 'R' || answer[0] =='r')
        {        
          x = 0;
          free(answer);
          break;
        }
      free(answer);
    } while (true);
  }
  while (x == 1);
  
  free(list.keys);
  free(list2.keys);
}


void remove_from_cart(db_t *db)
{
  lo_t list = list_carts(db);
  if (list.keys == 0)
    {
      free(list.keys);
      return;
    }
  int vagn = get_valid_int("Vilken vagn vill du använda?\n", 1, list.max);  ;
  ioopm_list_t *item_list = lookup_cart(db, list.keys[vagn-1]);
  size_t list_size = ioopm_linked_list_size(item_list);
  if(list_size ==0)
    {
      printf("Din kundvagn är tom, det finns inga varor att returnera\n");
      free(list.keys);
      return;
    }
  char **cart_items = merch_array(item_list);
  for (int x = 0; x < list_size; x++)
    { int *amount = find_amount(item_list,cart_items[x]);
      printf("%d. %s, %d\n", x+1, cart_items[x], *amount);
    }

  int vara = get_valid_int("Vilken vara vill du returnera?", 1, list.max);
  int ta_bort = get_valid_amount_to_remove("Hur många vill du ta bort?", *find_amount(item_list,cart_items[vara-1]));
  item_t *item_being_removed = lookup_item(db, cart_items[vara-1]);
  item_being_removed->amount_in_carts-=ta_bort;
  reduce_amount(item_list, ta_bort, cart_items[vara-1]);
  
  free(list.keys);
  free(cart_items);
}

void checkout(db_t *db)
{
  lo_t list   = list_carts(db);  
  if (list.keys == NULL)
    {
      free(list.keys);
      return;
    }
  int vagn = get_valid_int("Vilken kundvagn vill du checka ut?", 1, list.max);
  elem_t *cart_info = ioopm_hash_table_lookup(db->carts,(elem_t)list.keys[vagn-1]);
  cart_t *cart = cart_info->ptr;
  if (ioopm_linked_list_size(cart->items) == 0)
    {
      puts("Den här kundvagnen är tom");
      free(list.keys);
      return;
    }
  int cost = calculate_cost(cart,db);
  empty_cart(db, cart, true);
  free(list.keys);
  printf("Köpet gick igenom, du är nu %d.%d kronor fattigare\n", cost / 100, cost % 100);
}

void print_merch(db_t *db)
{
  lo_t list = list_ht(db);
  free(list.keys);
}
