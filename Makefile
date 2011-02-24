MAKE = make                                                                                                                                  
CFLAGS = -g -Wall -pedantic -std=c99 -lcurses
CC = gcc 

all: gangbang

config: config.c
	$(CC) $(CFLAGS) config.c -c -o config.o

gangbang: config gangbang.c
	$(CC) $(CFLAGS) config.o gangbang.c -o gangbang

 
clean:
	rm -f *.o gangbang

