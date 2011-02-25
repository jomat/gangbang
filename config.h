#ifndef GANGBANG_CONFIG_H
#define GANGBANG_CONFIG_H


#define _GNU_SOURCE
#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

struct confignet {
  char host[512];
  int port;
};

struct configoptwin {
  bool show;
  short fg;
  short bg;
};

struct configwin {
  short fg;
  short bg;
};

struct configkey {
  int pause, stop, love, next, ban, radio, discovery, quit;
};

struct lnf {
  int statusupdate;
};

struct config {
  struct confignet net;
  struct configwin history;
  struct configwin status;
  struct configoptwin info;
  struct configkey key;
  struct lnf lnf;
} config;

void read_config();

#endif /* GANGBANG_CONFIG_H */
