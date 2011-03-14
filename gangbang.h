/*
	Copyright (C) 2011 jomat <jomat+gangbang@jmt.gr>
	Published under the terms of the GNU General Public License (GPL).
*/

#ifndef GANGBANG_GANGBANG_H
#define GANGBANG_GANGBANG_H

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <menu.h>
#include <form.h>
#include <panel.h>
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
#include "util.h"
#include "net.h"
#include "config.h"
#include "updatestatus.h"

WINDOW *status, *history;
PANEL *pstatus, *phistory;
int oldLINES;

void refresh_main_screen();
void quit(void);
void addhistory(char *line);
void window_size_changed(void);
void keypresshandler(int key);
void create_windows();
void mainloop();
int main(void);

#endif /* GANGBANG_GANGBANG_H */
