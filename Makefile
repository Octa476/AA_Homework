CC = gcc
CFLAGS = -Wall -Werror -Wextra -g

all: build run

build: main.c piece_table.c piece_table.h
	$(CC) main.c piece_table.c -o main $(CFLAGS)
run: main
	./main
clean:
	rm -f main *.o