#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <stdlib.h>
#include <ctype.h>

WINDOW *status,*history,*info;

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
  int pause,stop,love,next,ban,radio,discovery,quit;
};

struct config {
  struct confignet net;
  struct configwin history;
  struct configwin status;
  struct configwin info;
  struct configkey key;
} config;

void quit(void)
{
  delwin(status);
  delwin(history);
  delwin(info);
  endwin();
}


void trim(char * s) {
  char * p = s;
  int l = strlen(p);

  while(isspace(p[l - 1])) p[--l] = 0;
  while(* p && isspace(* p)) ++p, --l;

  memmove(s, p, l + 1);
}

void read_config()
{
  FILE *fd;
  char *line = NULL;
  size_t len;
  ssize_t read;
  char configfile[FILENAME_MAX];
  char *key,*keyval,*saveptr=0;
  snprintf(configfile,FILENAME_MAX,"%s/%s",getenv("HOME"),".gangbangrc");

  strcpy(config.net.host,"schwester.club.muc.ccc.de");
  config.net.port=54311;

  fd=fopen(configfile,"r");
  if (fd == NULL)
  {
    fprintf(stderr,"%s could not be opened\n"
      "net.host = schwester.club.muc.ccc.de\n"
      "net.port = 54311\n"
      "window.history.foreground = BLACK\n"
      "window.history.background = RED\n"
      "window.history.show = TRUE\n"
      "window.status.foreground = BLACK\n"
      "window.status.background = GREEN\n"
      "window.status.show = TRUE\n"
      "window.info.foreground = BLACK\n"
      "window.info.background = BLUE\n"
      "window.info.show = TRUE\n"
      "key.pause = p\n"
      "key.stop = S\n"
      "key.love = l\n"
      "key.next = n\n"
      "key.ban = B\n"
      "key.radio = r\n"
      "key.discovery = d\n"
      "key.quit = Q\n"
      ,configfile);
    exit(EXIT_FAILURE);
  }

  while((read=getline(&line,&len,fd)) != -1)
  {
    if(line[0]=='#')
      continue;

    key=strtok_r(line,"=",&saveptr);
    if(key==NULL)
      continue;
    trim(key);
    keyval=strtok_r(NULL,"=",&saveptr);
    if(keyval==NULL)
      continue;
    trim(keyval);

    if(!strcmp(key,"net.host")) {
      printf("host %s\n",keyval);
    } else if(!strcmp(key,"net.port")) {
      printf("config.net.port\n");
    }

  }
}

void addhistory(char *line)
{
  int histsizey,histsizex;
  getmaxyx(history, histsizey, histsizex);

  scroll(history);

  mvwinsstr(history, histsizey-1, 0, line); 
  wrefresh(history);
}

void keypresshandler(int key)
{
  char line[COLS+1];
  snprintf(line,sizeof(line),"keypressed: %c",key);
  addhistory(line);
}

int main(void)
{

  read_config();
  return 0;

  int key=0;  // just pressed key for main loop

  initscr();
  atexit(quit);
  clear();
  noecho();
  curs_set(0);
  cbreak();
  keypad(stdscr, 1);

  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLUE);
  init_pair(2, COLOR_BLUE, COLOR_WHITE);
  init_pair(3, COLOR_BLACK, COLOR_GREEN);
  init_pair(4, COLOR_RED, COLOR_BLACK);

  history = newwin(LINES-5, COLS, 0, 0);
  status = newwin(4, COLS, LINES-5, 0);
  scrollok(history, TRUE);
  info = newwin(1, COLS, LINES-1, 0);
  
  bkgd(COLOR_PAIR(1));
  wbkgd(status, COLOR_PAIR(2));
  wbkgd(history, COLOR_PAIR(3));
  wbkgd(info, COLOR_PAIR(4));

  mvwaddstr(info, 0, 0, "pause - Stop - love - next - Ban - change radio - discovery - Quit");

  refresh();
  wrefresh(status);
  wrefresh(history);
  wrefresh(info);
 
  while((key=getch()) != 'Q')
  {
    keypresshandler(key);
  } 

  return(0);
}

