MAKE = make                                                                                                                                  
CFLAGS = -g -Wall -pedantic -std=c99 -lcurses -lmenu -lform -lpanel -lpthread
CC = gcc 

all: gangbang

updatestatus: updatestatus.c
	$(CC) $(CFLAGS) updatestatus.c -c -o updatestatus.o


config: config.c
	$(CC) $(CFLAGS) config.c -c -o config.o

net: net.c
	$(CC) $(CFLAGS) net.c -c -o net.o

gangbang: config net updatestatus gangbang.c
	$(CC) $(CFLAGS) net.o config.o updatestatus.o gangbang.c -o gangbang

 
clean:
	rm -f *.o gangbang

