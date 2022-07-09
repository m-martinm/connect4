CC=gcc
CFLAGS=-Wall -std=c99 -Wno-missing-braces


game: main.c
	$(CC) main.c connect.c -o connect4.exe $(CFLAGS) -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

test:test.c
	$(CC) test.c connect.c -o test.exe $(CFLAGS) -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
