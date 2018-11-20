#pragma once
#include "backend.h"


///@brief takes input from user and creates a item from it
///@param db | the database 
///@returns pointer to a newly created item
item_t *input_item(db_t *db);

///@brief inserts an item into a db
///@param db | database to insert item into
void insert_item_to_db (db_t *db);


///@brief list all items in a database in lexographic order
///@param db | database to list
///returns a linked list of items and the amount of items listed
lo_t list_ht(db_t *db);


///@brief removes an item from a database
///@param db | database to remove item from
void remove_item_from_db(db_t *db);


///@brief edits a item in a database
///@param db | database to edit a item in
void edit_db(db_t *db);

///@brief shows all merchandise in a database
///@param db | database to show content of
void show_stock(db_t *db);

///@brief increase the amount of a item in a database
///@param db | database in question
void replenish_item(db_t *db);


///@brief adds a shopping cart to a database
///@param db | database to add cart to
void add_cart(db_t *db);

///@brief checks out a existing shopping cart from a database
///@param db | database to check out a cart from
void checkout(db_t *db);


///@brief removes all shelves containing an item from a database
///@param db | database to insert item into
///@param merch | removes all shelves containing merch
void remove_from_shelves(c_m *merch,db_t *db);


///@brief removes a cart from the database
///@param db | database to remove cart from
void remove_cart(db_t *db);


///@brief prints out all the items in the database
///@param db | database to print
void print_merch(db_t *db);

///@brief removes an item from a cart
///@param db | database in which to remove an item from a cart
void remove_from_cart(db_t *db);


///@brief adds an item to a cart
///@param db | database in which to add an item to a cart
void add_to_cart(db_t *db);


///@brief lists all the carts in a database
///@param db | database to list the carts of
lo_t list_carts(db_t *db);
