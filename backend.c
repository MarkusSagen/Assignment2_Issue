#include "list_linked.h"
#include "hash_table.h"
#include "backend.h"
#include "utils.h"
#include "frontend.h"



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

bool compare_int_elements(elem_t *a, elem_t *b)
{
  return (a->int_value_t == b->int_value_t);  
}

bool compare_str_elements(elem_t *a, elem_t *b)
{
  return strcmp(a->string_value_t, b->string_value_t) == 0;  
}

db_t *create_db()
{
  ioopm_hash_table_t *merch_ht= ioopm_hash_table_create(string_to_hash,compare_str_elements,NULL);
  ioopm_hash_table_t *shelf_ht= ioopm_hash_table_create(string_to_hash,compare_str_elements,NULL);
  ioopm_hash_table_t *cart_ht= ioopm_hash_table_create(string_to_hash,compare_str_elements,NULL);
  db_t *db=calloc(1,sizeof(db_t));
  db->merch=merch_ht;
  db->shelf=shelf_ht;
  db->carts=cart_ht;
  return db;
}



void destroy_cart_ht(db_t *db)
{
  if (ioopm_hash_table_size(db->carts) == 0)
    {
      ioopm_hash_table_destroy(db->carts);
      return;
    }
  ioopm_list_t *list_of_carts = ioopm_hash_table_values(db->carts);
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(list_of_carts);
  size_t list_size = ioopm_linked_list_size(list_of_carts);
  elem_t current = ioopm_iterator_current(iterator);
  int i = 0;
  while (i < list_size)
    {
      cart_t *cart = current.ptr;      
      size_t list_size2 = ioopm_linked_list_size(cart->items);
      if (list_size2 == 0)
        {        
          ioopm_linked_list_destroy(cart->items);
          free(cart->id_name);
          free(cart);
          ioopm_iterator_next(iterator);
          current = ioopm_iterator_current(iterator);
          ++i;
        }
      else
        {
          ioopm_list_iterator_t *iterator2 = ioopm_list_iterator(cart->items);
          elem_t current2 = ioopm_iterator_current(iterator2);
          int x = 0;
          while(x < list_size2)
            {
              c_m *cm     = current2.ptr;
              elem_t *ptr = ioopm_iterator_next(iterator2);
              if(ptr!=NULL)
                {
                  current2 = ioopm_iterator_current(iterator2);
                }
              free(cm);
              ++x;
            }
          ioopm_linked_list_destroy(cart->items);
          ioopm_iterator_next(iterator);
          current = ioopm_iterator_current(iterator);
          free(cart->id_name);
          free(cart);
          ++i;
          ioopm_iterator_destroy(iterator2);
        }
    }
  ioopm_linked_list_destroy(list_of_carts);
  ioopm_iterator_destroy(iterator);
  
  ioopm_hash_table_destroy(db->carts);
}


void destroy_merch_ht(db_t *db)
{
  if (ioopm_hash_table_size(db->merch)==0)
    {
      ioopm_hash_table_destroy(db->merch);
      return;
    }
  ioopm_list_t *values=ioopm_hash_table_values(db->merch);
  size_t list_size=ioopm_linked_list_size(values);
  ioopm_list_iterator_t *iterator= ioopm_list_iterator(values);
  elem_t current= ioopm_iterator_current(iterator);
  int i=0;
  while (i< list_size)
    {
     
      item_t *item=current.ptr;
      ioopm_iterator_next(iterator);
      current=ioopm_iterator_current(iterator);
      size_t list_size2= ioopm_linked_list_size(item->shelfs_t);
      ioopm_list_iterator_t *iterator2 = ioopm_list_iterator(item->shelfs_t);
      elem_t current2 = ioopm_iterator_current(iterator2);
      int x=0;
      while(x < list_size2)
        {
          storage_t *stor = current2.ptr;
          elem_t *ptr     = ioopm_iterator_next(iterator2);
          if(ptr!=NULL)
            {
              current2=ioopm_iterator_current(iterator2);
            }
          free(stor->shelf);
          free(stor);
          ++x;
        }
      ioopm_linked_list_destroy(item->shelfs_t);
      free(item->name);
      free(item->desc);
      free(item);
      ++i;
      ioopm_iterator_destroy(iterator2);
    }
  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(values);
  ioopm_hash_table_destroy(db->merch);
}

void destroy_db(db_t *db)
{
  destroy_merch_ht(db);
  ioopm_hash_table_destroy(db->shelf);
  destroy_cart_ht(db);
  free(db);                                    
}


int *find_amount(ioopm_list_t *list,char *shelfs)
{

  size_t size = ioopm_linked_list_size(list);
  ioopm_list_iterator_t *iter_list= ioopm_list_iterator(list);
  elem_t shelf = ioopm_iterator_current(iter_list);

  for (int i = 0; i < size; i++)
    {
      storage_t *sh=shelf.ptr;
      bool result = compare_str_elements(&(elem_t){.string_value_t=shelfs},&(elem_t){.string_value_t=sh->shelf});
      if (result)
        {
          ioopm_iterator_destroy(iter_list);
          return &sh->amount;
        }
      ioopm_iterator_next(iter_list);
      shelf = ioopm_iterator_current(iter_list);
    }
  ioopm_iterator_destroy(iter_list);
  return 0;
}

int cmpstringp(const void *p1, const void *p2)
{
  return strcmp(* (char * const *) p1, * (char * const *) p2);
}

int cmpshelfsp(const void *p1, const void *p2)
{
  char *shelf1= *(char* const *) p1;
  char *shelf2= *(char* const *)p2;
  int s1_value=0;
  int s2_value=0;
  for(int i=0; i<strlen(shelf1); i++)
    {
      s1_value+=shelf1[i];
    }
  for (int x=0; x<strlen(shelf2); x++)
    {
      s2_value+=shelf2[x];
    }
  if (s1_value > s2_value)
    {
      return 1;
    }
  else if (s1_value == s2_value)
    {
      return 0;
    }
  else
    {
      return -1;
    }

}

void sort_shelfs(char *shelfs[], size_t no_shelfs)
{
  qsort(shelfs,no_shelfs,sizeof(char*),cmpshelfsp);
}
void sort_keys(char *keys[], size_t no_keys)
{
  qsort(keys, no_keys, sizeof(char *), cmpstringp);
}

item_t *make_item(char *namn, char *beskrivning, int pris, char *hylla, int amount)
{
  
  ioopm_list_t *list =ioopm_linked_list_create(NULL); 
  item_t *item_to_insert= calloc(1,sizeof(item_t));
  storage_t *stor=calloc(1,sizeof(storage_t));
  item_to_insert->name=namn;
  item_to_insert->desc=beskrivning;
  item_to_insert->price=pris;
  item_to_insert->total_amount_in_stock=amount;
  item_to_insert->amount_in_carts= 0;
  stor->shelf=hylla;
  stor->amount= amount;
  ioopm_linked_list_prepend(list,(elem_t){.ptr=stor});
  item_to_insert->shelfs_t=list;
  return item_to_insert;
}

void insert (db_t *db, item_t *item_to_insert)
{
  if (item_to_insert)
    {
      ioopm_list_t *list= item_to_insert->shelfs_t;     
      ioopm_list_iterator_t *iter_list=ioopm_list_iterator(list);
      elem_t stor = ioopm_iterator_current(iter_list);
      ioopm_hash_table_insert(db->merch,(elem_t)item_to_insert->name, (elem_t){.ptr=item_to_insert});
    
    
      storage_t *storage= stor.ptr;
      ioopm_hash_table_insert(db->shelf,(elem_t)storage->shelf,(elem_t)item_to_insert->name);    
    
      ioopm_iterator_destroy(iter_list);
    }
}

int find_index_for_item(ioopm_list_t *list, char*item)
{
  size_t size = ioopm_linked_list_size(list);
  ioopm_list_iterator_t *iter_list= ioopm_list_iterator(list);
  elem_t cart_merch = ioopm_iterator_current(iter_list);
  int index=0;
  for (int i = 0; i < size; i++)
    { c_m *it=cart_merch.ptr;
      bool result = compare_str_elements(&(elem_t){.string_value_t=item},&(elem_t){.string_value_t=it->merch});
      if (result)
        {
          ioopm_iterator_destroy(iter_list);
          return index;
        }
      ioopm_iterator_next(iter_list);
      cart_merch = ioopm_iterator_current(iter_list);
      index++;
    }
  ioopm_iterator_destroy(iter_list);
  return index;

}

static bool item_in_cart(cart_t *cart, char *name)
{
  if (ioopm_linked_list_size(cart->items) == 0) return false;
  ioopm_list_t *merch_in_current_cart = cart->items;
  size_t amount_of_merch = ioopm_linked_list_size(merch_in_current_cart);  
  ioopm_list_iterator_t *iterator_2 = ioopm_list_iterator(merch_in_current_cart);  
  elem_t current_item = ioopm_iterator_current(iterator_2);
  int n = 0;
  while (n < amount_of_merch)
    {
      c_m *merch = current_item.ptr;
      if (name == merch->merch)
        {
          ioopm_iterator_destroy(iterator_2);
          return true;
        }
      ioopm_iterator_next(iterator_2);
      current_item = ioopm_iterator_current(iterator_2);
      ++n;
    }
  ioopm_iterator_destroy(iterator_2);
  return false;
}

bool item_is_in_carts(db_t *db, char* name)
{
  if (ioopm_hash_table_size(db->carts) == 0) return false;
  ioopm_list_t *carts = ioopm_hash_table_values(db->carts);
  size_t amount_of_carts = ioopm_linked_list_size(carts);  
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(carts);  
  elem_t current_cart = ioopm_iterator_current(iterator);
  int i = 0;
  while (i < amount_of_carts)
    {
      cart_t *current_cart_t = current_cart.ptr;
      if (item_in_cart(current_cart_t, name))
        {
          
          ioopm_linked_list_destroy(carts);
          ioopm_iterator_destroy(iterator);  
          return true;
        }
      ioopm_iterator_next(iterator);
      current_cart = ioopm_iterator_current(iterator);
      ++i;
    }
  ioopm_linked_list_destroy(carts);
  ioopm_iterator_destroy(iterator);  
  return false;
}


void destroy_merch (db_t *db, char *to_remove) 
{    
  elem_t *elem_to_remove = ioopm_hash_table_lookup(db->merch,(elem_t)to_remove);
  item_t *item_to_remove = (item_t*)elem_to_remove->ptr;
  ioopm_list_t *shelves_to_remove = item_to_remove->shelfs_t;
  size_t list_size= ioopm_linked_list_size(shelves_to_remove);
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(shelves_to_remove);
  elem_t shelf = ioopm_iterator_current(iterator);
  for (int i = 0; i < list_size; i++)
    {
      storage_t *shelf_to_remove = shelf.ptr;
      ioopm_hash_table_remove(db->shelf,(elem_t)shelf_to_remove->shelf);
      elem_t *ptr = ioopm_iterator_next(iterator);
      if (ptr !=NULL) shelf = ioopm_iterator_current(iterator);
      free(shelf_to_remove->shelf);
      free(shelf_to_remove);
    }
  ioopm_iterator_destroy(iterator);
  ioopm_linked_list_destroy(shelves_to_remove);
  ioopm_hash_table_remove(db->merch,(elem_t)to_remove);
  free(item_to_remove->name);
  free(item_to_remove->desc);
  free(item_to_remove);
}


void edit_merch_in_db(db_t *db, ioopm_list_t *shelf_list, item_t *new_merch, item_t *old_item)
{
  size_t list_size = ioopm_linked_list_size(shelf_list);
  ioopm_list_iterator_t *iter_list = ioopm_list_iterator(shelf_list);
  elem_t sh = ioopm_iterator_current(iter_list);
  for (int i = 0; i<list_size; i++)
    {
      storage_t *shelf= sh.ptr;
      
      ioopm_hash_table_insert(db->shelf,(elem_t)shelf->shelf,(elem_t)new_merch->name);
      elem_t *ptr =ioopm_iterator_next(iter_list);
      if (ptr !=NULL) sh = ioopm_iterator_current(iter_list);
      
    }
  ioopm_iterator_destroy(iter_list);
  ioopm_hash_table_remove(db->merch, (elem_t)old_item->name);
  free(old_item->name);
  free(old_item->desc);
  
  free(old_item);
  ioopm_hash_table_insert(db->merch,(elem_t)new_merch->name,(elem_t){.ptr=new_merch});
}


item_t *create_item(char *name, char* description, int price, ioopm_list_t *stock, int total_amount_in_stock)
{
  item_t *new_item = calloc(1,sizeof(item_t));
  new_item->name=name;
  new_item->desc=description;
  new_item->price=price;
  new_item->shelfs_t=stock;
  new_item->total_amount_in_stock = total_amount_in_stock;
    
  return new_item;
}

item_t *lookup_item(db_t *db, char *merch_name)
{
  elem_t *elemptr = ioopm_hash_table_lookup(db->merch,(elem_t)merch_name);
  item_t *item = elemptr->ptr;
  return item;
}

ioopm_list_t *lookup_cart(db_t *db, char *cart_name)
{
  elem_t *elemptr = ioopm_hash_table_lookup(db->carts,(elem_t)cart_name);
  cart_t *cart = elemptr->ptr;
  return cart->items;
}


size_t db_size(db_t *db)
{
  size_t size = ioopm_hash_table_size(db->merch);
  if (size == 0)
    {
      printf("\nVaruhuset är tomt\n\n");
      return 0;
    }
  return size;
}

char **shelf_array(ioopm_list_t *linked_list_of_shelves)
{
  size_t list_size = ioopm_linked_list_size(linked_list_of_shelves);
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(linked_list_of_shelves);
  elem_t current_shelf = ioopm_iterator_current(iterator);
  char **array_of_shelves = calloc(list_size, sizeof(char *));
  int i = 0;
  
  while  (i < list_size)
    {
      storage_t *storage= current_shelf.ptr;
      array_of_shelves[i] = storage->shelf;
      ++i;
      elem_t *ptr = ioopm_iterator_next(iterator);
      if (ptr !=NULL) current_shelf = ioopm_iterator_current(iterator); 
    }
  ioopm_iterator_destroy(iterator);
  return array_of_shelves;
}
char **merch_array(ioopm_list_t *linked_list_of_merch)
{
  size_t list_size = ioopm_linked_list_size(linked_list_of_merch);
  ioopm_list_iterator_t *iterator = ioopm_list_iterator(linked_list_of_merch);
  elem_t current_merch = ioopm_iterator_current(iterator);
  char **array_of_merch = calloc(list_size, sizeof(char *));
  int i = 0;
  
  while  (i < list_size)
    {
      c_m *merch= current_merch.ptr;
      array_of_merch[i] = merch->merch;
      ++i;
      elem_t *ptr = ioopm_iterator_next(iterator);
      if (ptr !=NULL) current_merch = ioopm_iterator_current(iterator); 
    }
  ioopm_iterator_destroy(iterator);
  return array_of_merch;
}


void print_stock(item_t *item_to_show, char **sorted_array_of_shelves)
{
  for (int x = 0; x < ioopm_linked_list_size(item_to_show->shelfs_t); x++)
    {
      int *amount = find_amount(item_to_show->shelfs_t, sorted_array_of_shelves[x]);
      printf("%d. %s, %d\n",x+1, sorted_array_of_shelves[x],*amount);
    }
  printf("Total stock: %d\n",item_to_show->total_amount_in_stock); 
}

void remove_from_shelves(c_m *merch,db_t *db)
{
  elem_t *item_info = ioopm_hash_table_lookup(db->merch,(elem_t)merch->merch);
  item_t *item = item_info->ptr;
  ioopm_list_t *shelves = item->shelfs_t;
  while(merch->amount > 0)
    {
      elem_t *stor = ioopm_linked_list_get(shelves,0);
      storage_t *storage = stor->ptr;
      if(merch->amount >= storage->amount)
        {
          merch->amount -= storage->amount;
          item->amount_in_carts -= storage->amount;
          ioopm_hash_table_remove(db->shelf,(elem_t)storage->shelf);
          free(storage->shelf);
          free(storage);
          ioopm_linked_list_remove(shelves,0);
        }
      else
        {
          storage->amount-=merch->amount;
          item->amount_in_carts-=merch->amount;
          merch->amount=0;          
        }
    }
}

void empty_cart(db_t *db, cart_t *cart, bool checkout)
{
  ioopm_list_t *item_list = cart->items;
  size_t list_size = ioopm_linked_list_size(item_list);
  if (!list_size) return;
  ioopm_list_iterator_t *iter_cart_items =ioopm_list_iterator(item_list);
  elem_t current= ioopm_iterator_current(iter_cart_items);
  int i = 0;
  while (i < list_size)
    {
      c_m *cart_merch = current.ptr;
      item_t *item_info = lookup_item(db, cart_merch->merch);
      item_info->amount_in_carts -= cart_merch->amount;
      if (checkout)
        {
          item_info->total_amount_in_stock -= cart_merch->amount;         
          remove_from_shelves(cart_merch, db);
          if(item_info->total_amount_in_stock == 0)
            {
              ioopm_linked_list_destroy(item_info->shelfs_t);
              ioopm_hash_table_remove(db->merch,(elem_t)item_info->name);     
              free(item_info->name);
              free(item_info->desc);
              free(item_info);
            }         
        }
      ioopm_iterator_next(iter_cart_items);
      current=ioopm_iterator_current(iter_cart_items);
      free(cart_merch);
      i++;
    }
  ioopm_iterator_destroy(iter_cart_items);
  ioopm_linked_list_destroy(cart->items);
  if (checkout)
    {
      ioopm_hash_table_remove(db->carts,(elem_t)cart->id_name);
      free(cart->id_name);
      free(cart);
      
    }
  else
    {
      cart->items = ioopm_linked_list_create(compare_str_elements);
    }
}


int calculate_cost(cart_t *cart,db_t *db)
{
  ioopm_list_t *cart_list=cart->items;
  size_t size= ioopm_linked_list_size(cart_list);
  ioopm_list_iterator_t *iter_list=ioopm_list_iterator(cart_list);
  elem_t current=ioopm_iterator_current(iter_list);
  int cost=0;
  for (int i = 0;i<size;i++)
    {
      c_m *merch=current.ptr;
      elem_t *item_info=ioopm_hash_table_lookup(db->merch,(elem_t)merch->merch);
      item_t *item=item_info->ptr;
      cost+=item->price*merch->amount;
      ioopm_iterator_next(iter_list);
      current=ioopm_iterator_current(iter_list);
    }
  ioopm_iterator_destroy(iter_list);
  return cost;
}


void reduce_amount(ioopm_list_t *list_of_merch,int amount_to_remove, char* name_of_merch)
{
  int index = find_index_for_item(list_of_merch,name_of_merch);
  if(amount_to_remove==*find_amount(list_of_merch, name_of_merch))
    {
      c_m *to_remove = ioopm_linked_list_get(list_of_merch,index)->ptr;     
      ioopm_linked_list_remove(list_of_merch,index);    
      free(to_remove);    
    }
  else
    {
      elem_t *merch=ioopm_linked_list_get(list_of_merch,index);
      c_m *cart=merch->ptr;
      cart->amount-=amount_to_remove;
    }
}

bool item_in_list(ioopm_list_t *list, char *item)
{
  size_t size = ioopm_linked_list_size(list);
  if (size==0)
    {
      return false;
    }
  ioopm_list_iterator_t *iter_list= ioopm_list_iterator(list);
  elem_t cart_merch = ioopm_iterator_current(iter_list);
  bool result;
  for (int i = 0; i < size; i++)
    {
      c_m *it=cart_merch.ptr;
      result = compare_str_elements(&(elem_t){.string_value_t=item},&(elem_t){.string_value_t=it->merch});
      if (result)
        {
          ioopm_iterator_destroy(iter_list);
          return result;
        }
      ioopm_iterator_next(iter_list);
      cart_merch = ioopm_iterator_current(iter_list);      
    }
  ioopm_iterator_destroy(iter_list);
  return result;
}

void update_stock_of_item(db_t *db, char *name, char *shelf, int amount)
{
      storage_t *stock = calloc(1,sizeof(storage_t));
      stock->shelf=shelf;
      stock->amount=amount;
      item_t *item = lookup_item(db,name);
      item->total_amount_in_stock+=amount;
      ioopm_list_t *item_list=item->shelfs_t;
      ioopm_linked_list_prepend(item_list,(elem_t){.ptr=stock});
      ioopm_hash_table_insert(db->shelf,(elem_t)shelf,(elem_t)name);
}

void add_cart_to_db(db_t* db, char *id)
{
  ioopm_list_t *item_list = ioopm_linked_list_create(compare_str_elements);

  cart_t *cart  = calloc(1,sizeof(cart_t));
  cart->id_name = id;
  cart->items   = item_list;

  ioopm_hash_table_insert(db->carts,(elem_t)id,(elem_t){.ptr=cart});  
}

void add_item_to_cart(cart_t *cart, item_t *merch, int amount)
{ 
  merch->amount_in_carts += amount;  
  if(item_in_list(cart->items,merch->name))
    {
      int index_for_item = find_index_for_item(cart->items,merch->name);
      elem_t *item_info  = ioopm_linked_list_get(cart->items,index_for_item);
      c_m *item_in_cart  = item_info->ptr;
      item_in_cart->amount  += amount;
    }
  else
    {
      c_m *item_in_cart    = calloc(1,sizeof(c_m));
      item_in_cart->merch  = merch->name;
      item_in_cart->amount = amount;
      ioopm_linked_list_prepend(cart->items,(elem_t){.ptr=item_in_cart});
    }
}
