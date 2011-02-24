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
#include "net.h"
#include "config.h"

WINDOW *status, *history, *info;

void quit(void)
{

  delwin(status);
  delwin(history);
  delwin(info);
  endwin();

}

void update_status()
{

  int n;
  int sock_status;
  char buf[1024];
  sock_status = socket_connect(config.net.host, config.net.port);
  n = write(sock_status, "info %a|%t|%A|%d|%s|%u|%U|%X|%T|%R\n", 36);
  if (n < 0) {
    mvwaddstr(status, 0, 0, "cant write to remote");
    wrefresh(status);
  } else {
    while (read(sock_status, buf, sizeof(buf) - 1)) {
      mvwaddstr(status, 0, 0, buf);
      wrefresh(status);
    }
  }
  close(sock_status);

  wrefresh(status);

  wclear(status);

}

void addhistory(char *line)
{
  int histsizey, histsizex;
  getmaxyx(history, histsizey, histsizex);

  scroll(history);

  mvwinsstr(history, histsizey - 1, 0, line);
  wrefresh(history);
}

void keypresshandler(int key)
{
  char line[COLS + 1];
  snprintf(line, sizeof(line), "keypressed: %c", key);
  addhistory(line);
  if (key == 'c')
    update_status();
}

void create_windows()
{
  char tmp[1024];
  initscr();
  clear();
  noecho();
  curs_set(0);
  cbreak();
  keypad(stdscr, 1);

  start_color();
  use_default_colors();
  init_pair(1, COLOR_RED, COLOR_BLUE);
  init_pair(2, config.status.fg, config.status.bg);
  init_pair(3, config.history.fg, config.history.bg);
  init_pair(4, config.info.fg, config.info.bg);

  history = newwin(LINES - (4 + config.info.show), COLS, 0, 0);
  status = newwin(4, COLS, LINES - (4 + config.info.show), 0);
  scrollok(history, TRUE);
  info = newwin(1, COLS, LINES - 1, 0);

  bkgd(COLOR_PAIR(1));
  wbkgd(status, COLOR_PAIR(2));
  wbkgd(history, COLOR_PAIR(3));
  wbkgd(info, COLOR_PAIR(4));

  snprintf(tmp, sizeof(tmp),
           "%c: pause - %c: stop - %c: love - %c: next - %c: ban "
           "- %c: change radio - %c: discovery - %c: quit",
           config.key.pause, config.key.stop, config.key.love,
           config.key.next, config.key.ban, config.key.radio,
           config.key.discovery, config.key.quit);
  mvwaddstr(info, 0, 0, tmp);

  refresh();
  wrefresh(status);
  wrefresh(history);
  if (config.info.show)
    wrefresh(info);

}

void mainloop()
{
  int key = 0;
  while ((key = getch()) != config.key.quit) {
    keypresshandler(key);
  }

}

int main(void)
{

  atexit(quit);

  read_config();

  create_windows();

  mainloop();

  return (0);
}

