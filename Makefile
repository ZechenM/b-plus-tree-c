CC=gcc
CFLAGS=-Wall -g

run: bptree
	./bptree

bptree: main.o bptree.o
	$(CC) $(CFLAGS) -o bptree main.o bptree.o

main.o: main.c bptree.h
	$(CC) $(CFLAGS) -c main.c

bptree.o: bptree.c bptree.h
	$(CC) $(CFLAGS) -c bptree.c

clean:
	rm *.o bptree