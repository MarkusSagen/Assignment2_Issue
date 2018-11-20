#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "backend.h"
#include "hash_table.h"
#include "CUnit/Basic.h"


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
struct item{
  char *name;
  char *desc;
  int price;
  ioopm_list_t *shelfs_t;
  int total_amount_in_stock;
  int amount_in_carts;
};

bool test_apply (int key, char *value, void *arg)
{
  
  printf("%d, %s\n",key, value);
  return true;
}

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

void test_db(void)
{
  db_t *test_db = create_db();
  destroy_db(test_db);
}

void test_items(void)
{
  db_t *test_db = create_db();
  char *name  = strdup("Choklad");
  char *desc  = strdup("Mörk");
  char *shelf = strdup("A1");
  item_t *test_item = make_item(name, desc, 1599, shelf, 100);
  insert(test_db, test_item);
  CU_ASSERT_PTR_NOT_NULL(lookup_item(test_db, name));
  
  char *name2  = strdup("Strumpor");
  char *desc2  = strdup("Gula");
  item_t *test_item2 = create_item(name2, desc2, 12599, test_item->shelfs_t, test_item->total_amount_in_stock);
  edit_merch_in_db(test_db, test_item->shelfs_t, test_item2, test_item);  
  CU_ASSERT_PTR_NOT_NULL(lookup_item(test_db, name2));

  elem_t *storage = ioopm_linked_list_get((lookup_item(test_db, "Strumpor")->shelfs_t),0);
  storage_t *stor = storage->ptr;
  CU_ASSERT_STRING_EQUAL("A1", stor->shelf);
  CU_ASSERT_EQUAL(100, stor->amount);
  CU_ASSERT_EQUAL(ioopm_hash_table_size(test_db->merch), 1);
  
  destroy_merch(test_db, "Strumpor");
  CU_ASSERT_EQUAL(ioopm_hash_table_size(test_db->merch), 0);
  
  destroy_db(test_db);
}

void test_lists(void)
{
  db_t *test_db = create_db();
  char *name  = strdup("Choklad");
  char *desc  = strdup("Mörk"); 
  char *shelf = strdup("A1");
  char *shelf2 = strdup("A2");
  char *shelf3 = strdup("A3");
  char *shelf4 = strdup("A4");
  char *shelf5 = strdup("A5");
  item_t *test_item = make_item(name, desc, 1599, shelf, 100);
  insert(test_db, test_item);
  update_stock_of_item(test_db, name, shelf2, 1);
  update_stock_of_item(test_db, name, shelf3, 3);
  update_stock_of_item(test_db, name, shelf4, 670);
  update_stock_of_item(test_db, name, shelf5, 12);

  CU_ASSERT_EQUAL(*find_amount(test_item->shelfs_t, "A1"), 100);
  CU_ASSERT_EQUAL(*find_amount(test_item->shelfs_t, "A2"), 1);
  CU_ASSERT_EQUAL(*find_amount(test_item->shelfs_t, "A3"), 3);
  CU_ASSERT_EQUAL(*find_amount(test_item->shelfs_t, "A4"), 670);
  CU_ASSERT_EQUAL(*find_amount(test_item->shelfs_t, "A5"), 12);

  char **sorted_shelves = shelf_array(test_item->shelfs_t);
  sort_shelfs(sorted_shelves, 5);

  CU_ASSERT_STRING_EQUAL(sorted_shelves[0], "A1");
  CU_ASSERT_STRING_EQUAL(sorted_shelves[1], "A2");
  CU_ASSERT_STRING_EQUAL(sorted_shelves[2], "A3");
  CU_ASSERT_STRING_EQUAL(sorted_shelves[3], "A4");
  CU_ASSERT_STRING_EQUAL(sorted_shelves[4], "A5");

  char *name2  = strdup("Godis");
  char *desc2  = strdup("Nyttigt"); 
  char *shelf_2 = strdup("B1");
  item_t *test_item2 = make_item(name2, desc2, 9999, shelf_2, 123);
  
  char *name3  = strdup("Dator");
  char *desc3  = strdup("Mac"); 
  char *shelf_3 = strdup("C1");
  item_t *test_item3 = make_item(name3, desc3, 9999, shelf_3, 1234);
  
  char *name4  = strdup("Kaffe");
  char *desc4  = strdup("need"); 
  char *shelf_4 = strdup("D1");
  item_t *test_item4 = make_item(name4, desc4, 9999, shelf_4, 12345);

  insert(test_db, test_item2);
  insert(test_db, test_item3);
  insert(test_db, test_item4);

  ioopm_list_t *list_of_merch = ioopm_hash_table_keys(test_db->merch);

  CU_ASSERT_EQUAL(ioopm_linked_list_size(list_of_merch), 4);
    
  CU_ASSERT(ioopm_linked_list_contains(list_of_merch, (elem_t)name));
  CU_ASSERT(ioopm_linked_list_contains(list_of_merch, (elem_t)name2));
  CU_ASSERT(ioopm_linked_list_contains(list_of_merch, (elem_t)name3));
  CU_ASSERT(ioopm_linked_list_contains(list_of_merch, (elem_t)name4));
  
  ioopm_linked_list_destroy(list_of_merch);
  
  free(sorted_shelves);
  destroy_db(test_db);
}


void test_carts(void)
{
  db_t *test_db = create_db();
  char *name  = strdup("Choklad");
  char *desc  = strdup("Mörk"); 
  char *shelf = strdup("A1");
  char *shelf2 = strdup("A2");
  char *shelf3 = strdup("A3");
  char *shelf4 = strdup("A4");
  char *shelf5 = strdup("A5");
  item_t *test_item = make_item(name, desc, 1599, shelf, 100);
  insert(test_db, test_item);
  update_stock_of_item(test_db, name, shelf2, 1);
  update_stock_of_item(test_db, name, shelf3, 3);
  update_stock_of_item(test_db, name, shelf4, 670);
  update_stock_of_item(test_db, name, shelf5, 12);

  char *name2  = strdup("Godis");
  char *desc2  = strdup("Nyttigt"); 
  char *shelf_2 = strdup("B1");
  item_t *test_item2 = make_item(name2, desc2, 9999, shelf_2, 123);
  
  char *name3  = strdup("Dator");
  char *desc3  = strdup("Mac"); 
  char *shelf_3 = strdup("C1");
  item_t *test_item3 = make_item(name3, desc3, 9999, shelf_3, 1234);
  
  char *name4  = strdup("Kaffe");
  char *desc4  = strdup("need"); 
  char *shelf_4 = strdup("D1");
  item_t *test_item4 = make_item(name4, desc4, 9999, shelf_4, 12345);

  insert(test_db, test_item2);
  insert(test_db, test_item3);
  insert(test_db, test_item4);


  char *id = strdup("Johans kundvagn");
  add_cart_to_db(test_db, id);
  cart_t *cart = ioopm_hash_table_lookup(test_db->carts, (elem_t)"Johans kundvagn")->ptr;
  add_item_to_cart(cart, test_item, 100);

  CU_ASSERT(item_in_list(cart->items, name));
  CU_ASSERT(!item_in_list(cart->items, name2));
  CU_ASSERT(!item_in_list(cart->items, name3));
  CU_ASSERT(!item_in_list(cart->items, name4));
  
  add_item_to_cart(cart, test_item2, 2);
  add_item_to_cart(cart, test_item3, 3);
  add_item_to_cart(cart, test_item4, 2);
  add_item_to_cart(cart, test_item4, 2);
  
  CU_ASSERT(item_in_list(cart->items, name2));
  CU_ASSERT(item_in_list(cart->items, name3));
  CU_ASSERT(item_in_list(cart->items, name4));
  
  CU_ASSERT(*find_amount(cart->items, name2) == 2);
  CU_ASSERT(*find_amount(cart->items, name) == 100);
  CU_ASSERT(*find_amount(cart->items, name3) == 3);
  CU_ASSERT(*find_amount(cart->items, name4) == 4);
  CU_ASSERT(item_is_in_carts(test_db, name));
  CU_ASSERT(item_is_in_carts(test_db, name2));
  CU_ASSERT(item_is_in_carts(test_db, name3));
  CU_ASSERT(item_is_in_carts(test_db, name4));

  reduce_amount(cart->items, 50, name);
  CU_ASSERT(*find_amount(cart->items, name) == 50);
  reduce_amount(cart->items, 50, name);
  CU_ASSERT(!item_in_list(cart->items,name));

  empty_cart(test_db, cart, false);
  CU_ASSERT(ioopm_linked_list_size(cart->items) == 0);
  CU_ASSERT(ioopm_hash_table_size(test_db->merch) == 4);
  CU_ASSERT_FALSE(item_is_in_carts(test_db, name));
  
  add_item_to_cart(cart, test_item, 100);
  CU_ASSERT_EQUAL(calculate_cost(cart, test_db), 1599 * 100);
  add_item_to_cart(cart, test_item2, 1);
  add_item_to_cart(cart, test_item3, 1);
  add_item_to_cart(cart, test_item4, 1);


  char **array = merch_array(lookup_cart(test_db, "Johans kundvagn"));
  sort_keys(array, 4);

  CU_ASSERT_STRING_EQUAL(array[0], "Choklad");
  CU_ASSERT_STRING_EQUAL(array[1], "Dator");
  CU_ASSERT_STRING_EQUAL(array[2], "Godis");
  CU_ASSERT_STRING_EQUAL(array[3], "Kaffe");

  CU_ASSERT(4 == db_size(test_db));
  char *id_2 = strdup("Sebastians kundvagn");
  add_cart_to_db(test_db, id_2);
  CU_ASSERT(ioopm_hash_table_size(test_db->carts) == 2);
  cart_t *cart_2 = ioopm_hash_table_lookup(test_db->carts, (elem_t)"Sebastians kundvagn")->ptr;
  add_item_to_cart(cart_2, test_item4, 1);

  empty_cart(test_db, cart, true);
  CU_ASSERT(test_item->total_amount_in_stock > 0);
  CU_ASSERT(ioopm_hash_table_size(test_db->merch) == 4);
  free(array);
  destroy_db(test_db);
}

int main()
{
  CU_pSuite test_suite1 = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Test Suite 1", init_suite, clean_suite);
  if (NULL == test_suite1)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(test_suite1, "test create/destroy db", test_db))  ||
      (NULL == CU_add_test(test_suite1, "test item manipulation", test_items)) ||
      (NULL == CU_add_test(test_suite1, "test list manipulation", test_lists)) ||
      (NULL == CU_add_test(test_suite1, "test cart manipulation", test_carts)) 
      
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
