CC = gcc
CFLAGS = -W

btree: btree.c btree.h
	$(CC) -o btree -g btree.c $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o btree
