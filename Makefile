MAKE = make                                                                                                                                  
CFLAGS = -g -Wall -pedantic -std=c99 -lcurses
CC = gcc 

gangbang: gangbang.c
	$(CC) $(CFLAGS) gangbang.c -o gangbang

all:
	$(MAKE) gangbang
 
clean:
	rm -f *.o gangbang

