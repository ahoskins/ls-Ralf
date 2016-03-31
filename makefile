CC = gcc
CFLAGS = -g -std=c99 -D_BSD_SOURCE

main: main.c
	$(CC) main.c -lm -o lsRalF $(CFLAGS)

clean:
	rm -f lsRalF
	rm -rf lsRalF.dSYM
