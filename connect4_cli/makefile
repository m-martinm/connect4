CC=gcc
CFLAGS=-Wall -g -Werror
BINS=main
all: $(BINS)

test: test.c
	$(CC) $(CFLAGS) test.c connect.c -o test.out -lm && ./test.out

main: main.c connect.c
	$(CC) $(CFLAGS) main.c connect.c -o connect4.exe -lm && ./connect4.exe

clean: 
	rm *.out *.exe