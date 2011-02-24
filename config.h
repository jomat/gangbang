#define _GNU_SOURCE
#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

struct confignet {
  char host[512];
  int port;
};

struct configwin {
  bool show;
  short fg;
  short bg;
};

struct configkey {
  int pause, stop, love, next, ban, radio, discovery, quit;
};

struct config {
  struct confignet net;
  struct configwin history;
  struct configwin status;
  struct configwin info;
  struct configkey key;
} config;

void read_config();

