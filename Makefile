CC := gcc
CCFLAGS := -std=c17 -Wall -Wextra -Wpedantic -g

all:
	$(CC) *.c $(CCFLAGS) -o compiler.exe
	./compiler.exe test.amo
