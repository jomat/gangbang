#ifndef GANGBANG_GANGBANG_H
#define GANGBANG_GANGBANG_H

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <time.h>
#include "net.h"
#include "config.h"
#include "updatestatus.h"

WINDOW *status, *history, *info;

void quit(void);
void addhistory(char *line);
void keypresshandler(int key);
void create_windows();
void mainloop();
int main(void);

#endif /* GANGBANG_GANGBANG_H */
