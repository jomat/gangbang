MAKE = make                                                                                                                                  
CFLAGS = -g -Wall -pedantic -std=c99 -lcurses -lpthread
CC = gcc 

all: gangbang

config: config.c
	$(CC) $(CFLAGS) config.c -c -o config.o

net: net.c
	$(CC) $(CFLAGS) net.c -c -o net.o

gangbang: config net gangbang.c
	$(CC) $(CFLAGS) net.o config.o gangbang.c -o gangbang

 
clean:
	rm -f *.o gangbang

