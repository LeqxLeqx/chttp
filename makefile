

SRC_FILES=$(wildcard src/*.c)
OBJ_FILES=$(SRC_FILES:src/%.c=bin/%.o)

CFLAGS=-g -Wall -std=gnu11 -fPIC
CC=gcc


.PHONY : all
all : $(OBJ_FILES) bin
	$(CC) $(CFLAGS) $(OBJ_FILES) -shared -o bin/libchttp.so

bin/%.o : src/%.c bin
	$(CC) $(CFLAGS) -c -o $@ $<

bin : 
	mkdir bin


.PHONY : clean
clean :
	rm -rf bin

