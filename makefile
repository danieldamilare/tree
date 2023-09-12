CC = gcc
CFLAGS = -Wall -Wextra -g

tree: tree.o utils.o utils.h
	$(CC) $(CFLAGS) tree.o utils.o -o tree

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean: 
	rm -f utils.o tree.o
