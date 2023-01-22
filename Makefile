# -*- Makefile -*-

CC=/usr/bin/gcc
COMPILER_FLAGS=-Wall -g

bin/changelogs: build/main.o
	$(CC) $(COMPILER_FLAGS) build/main.o -o bin/changelogs

build/main.o: src/main.c
	$(CC) -c $(COMPILER_FLAGS) src/main.c -o build/main.o

clean:
	$(RM) build/* bin/*