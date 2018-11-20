main: utils.c iterator.c linked_list.c hash_table.c backend.c frontend.c
	gcc -Wall -g assignment2.c utils.c iterator.c linked_list.c frontend.c backend.c hash_table.c 

run: utils.c iterator.c linked_list.c hash_table.c backend.c frontend.c
	gcc -Wall -g assignment2.c utils.c iterator.c linked_list.c frontend.c backend.c hash_table.c && ./a.out

mem: utils.c iterator.c linked_list.c hash_table.c backend.c frontend.c
	gcc -Wall -g -ggdb assignment2.c utils.c iterator.c linked_list.c frontend.c backend.c hash_table.c && valgrind --leak-check=full ./a.out

test: utils.c iterator.c linked_list.c hash_table.c backend.c frontend.c
	gcc -coverage -Wall -g testbackend.c utils.c iterator.c linked_list.c hash_table.c backend.c frontend.c -o unittests -lcunit && valgrind --leak-check=full ./unittests

proftest: utils.c iterator.c linked_list.c hash_table.c backend.c frontend.c
	gcc -fprofile-arcs -pg testbackend.c utils.c iterator.c linked_list.c frontend.c backend.c hash_table.c ./a.out -lcunit -o proftest
