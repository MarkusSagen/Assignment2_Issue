#pragma once
#include "list_linked.h"

typedef struct item item_t;
typedef struct storage storage_t;
typedef struct database db_t;
typedef struct list_out lo_t;
typedef struct cart cart_t;
typedef struct cart_merch c_m;

///@brief creates a database containing three hashtables
db_t *create_db();

///@brief destroy a database and frees up its memory
///@param db | database to destroy
void destroy_db(db_t *db);


int cmpstringp(const void *p1, const void *p2);
int cmpshelfsp(const void *p1, const void *p2);

///TODO: FIXA FUNKTIONSSPECAR
///NYA FUNKTIONER
void add_cart_to_db(db_t* db, char *id);
void update_stock_of_item(db_t *db, char *name, char *shelf, int amount);
void add_item_to_cart(cart_t *cart, item_t *merch, int amount);
size_t db_size (db_t *db);

///@brief sorts an array of shelves in lexographic order
///@param shelfs | array to be sorted
///@param no_shelfs | length of the array
void sort_shelfs(char *shelfs[], size_t no_shelfs);

///@brief sorts an array of items in lexographic order
///@param shelfs | array to be sorted
///@param no_shelfs | length of the array
void sort_keys(char *keys[], size_t no_keys);


///@brief finds the amount of items on a specific shelf
///@param list | list of shelves containing a specific item
///@param shelfs | name of the shelf sought
///@return amount of merch on specified shelf
int *find_amount(ioopm_list_t *list, char* shelfs);

///@brief finds position of a item in a linked list
///@param list | linked list to search in
///@param item | item to search for
///returns the index of the item in the linked list
int find_index_for_item(ioopm_list_t *list, char*item);


///@brief finds position of a item in a linked list
///@param list | linked list to search in
///@param item | item to search for
///returns the index of the item in the linked list
void insert (db_t *db, item_t *item_to_insert);


///@brief removes merch from database and frees up it's allocated memory
///@param db | database to remove merch from
///@param list | list of merchandise for the user to choose from
///@param user_input | the users choice
///@param size_of_ht | total amount of merch in the database
void destroy_merch (db_t *db, char *to_remove);


///@brief edits a merchandise in the database
///@param db | database to remove merch from
///@param old_merch | the merch the user want to modify
///@param new_merch | new merch to replace the old merchandise
///@param shelf_list | list of shelves containing said old merchandise
void edit_merch_in_db (db_t *db, ioopm_list_t *shelf_list, item_t *new_merch, item_t *old_merch);

///@brief creates a new item
///@param name | name of merchandise
///@param description | string describing said merchandise
///@param price | price of merchandise
///@param stock | list of shelves containing merchandise, and amount stored there
///@param total_amount_in_stock | total amount of merchandise in the database
item_t *create_item(char *name, char* description, int price, ioopm_list_t *stock, int total_amount_in_stock);


item_t *make_item(char *namn, char *beskrivning, int pris, char *hylla, int amount);

///@brief looks up all information about a merchandise
///@param db | database to look for merch in
///@param merch_name | merch to look for
///@returns all information about the merch
item_t *lookup_item(db_t *db, char *merch_name);

///@brief looks up all items contained in a cart
///@param db | database to look for cart in
///@param cart_name| cart to look for
///@returns a linked list of all items in cart
ioopm_list_t *lookup_cart(db_t *db, char *cart_name);

///@brief a sorted array of all the shelves in a linked list
///@param linked_list_of_shelves | linked list to sort
///@returns a lexographically sorted array of names of shelves 
char **shelf_array(ioopm_list_t *linked_list_of_shelves);

///@brief a sorted array of all the merch in a linked list
///@param linked_list_of_merch | linked list to sort
///@returns a lexographically sorted array of names of merch 
char **merch_array(ioopm_list_t *linked_list_of_merch);

///@brief prints current stock of a merchandise in the terminal window
///@param item_to_show | merchandise which stock the user is seeking
///@param sorted_array_of_shelves | all shelves containing said stock in a sorted array
void print_stock(item_t *item_to_show, char **sorted_array_of_shelves);


///@brief checks if a merch is currently in a shopping cart
///@param db | database we are checking in
///@param name | name of merch sought
///@return true if merch is in a cart
bool item_is_in_carts(db_t *db, char* name);

///@brief empties a cart and returns the merch in it to the database
///@param db | database the cart is in
///@param cart | id of the cart
///@param checkout | true if used to check out
void empty_cart(db_t *db, cart_t *cart, bool checkout);


///@brief removes a merch in a shopping cart from a db
///@param db | database to remove merch from
///@param merch | merch in cart
void remove_from_shelves(c_m *merch,db_t *db);


///@brief calculates total cost of a shopping cart
///@param db | database with all the merchandise
///@param cart | cart containing merch to calculate cost of
///@returns cost in öre of all merch contained in cart
int calculate_cost(cart_t *cart,db_t *db);

///@brief reduces amount of a specific merch in a shopping cart
///@param list_of_merch | list containing all merch in the cart
///@param name_of_merch | name of the merch to be reduced
///@param amount_to_remove | amount to reduce merch with
void reduce_amount(ioopm_list_t *list_of_merch,int amount_to_remove, char* name_of_merch);


///@brief checks if a specific item is in a list
///@param list | list to check
///@param item | item sought
///@returns true if item is in the list, otherwise false
bool item_in_list(ioopm_list_t *list, char *item);
