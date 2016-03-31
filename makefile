CC = gcc
CFLAGS = -g -std=c99 -D_BSD_SOURCE

main: main.c
	$(CC) main.c -o lsRalf $(CFLAGS)

clean:
	rm -f lsRalf
	rm -rf lsRalf.dSYM
