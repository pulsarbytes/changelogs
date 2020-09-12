# -*- Makefile -*-

CC=/usr/bin/gcc
OFLAGS=-g
CFLAGS=-Wall -g

bin/changelogs: build/main.o
	$(CC) $(OFLAGS) build/main.o -o bin/changelogs

build/main.o: src/main.c
	$(CC) -c $(CFLAGS) src/main.c -o build/main.o

clean:
	$(RM) build/* bin/*