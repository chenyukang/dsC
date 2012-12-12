
CC = gcc  -g
LD = gcc 
TARGET = maze
SOURCES = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SOURCES))

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

bitmap_test:bitmap_test.o bitmap.o
	$(CC) bitmap_test.o bitmap.o -o bitmap_test

hashmap_test:hashmap_test.o hashmap.o
	$(CC) hashmap_test.o hashmap.o -o hashmap_test

chainhash_test:chainhash_test.o chainhash.o
	$(CC) chainhash_test.o chainhash.o -o chainhash_test

skip_list_test:skip_list_test.o	skiplist.o
	$(CC) skip_list_test.o skiplist.o -o skip_list_test

gcc_hashmap:gcc_hashmap.cpp
	g++ gcc_hashmap.cpp -o gcc_hashmap

all: bitmap_test hashmap_test chainhash_test skip_list_test gcc_hashmap
clean:
	rm -rf bitmap_test hashmap_test chainhash_test skip_list_test gcc_hashmap