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
} ;

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

void read_config(struct config *config)
{
  FILE *fd;
  char line[512];
  char configfile[FILENAME_MAX];
  char *key,*keyval,*saveptr=0;
  snprintf(configfile,FILENAME_MAX,"%s/%s",getenv("HOME"),".gangbangrc");

  strcpy(config->net.host,"schwester.club.muc.ccc.de");
  config->net.port=54311;
  config->history.show=TRUE;
  config->history.fg=COLOR_BLACK;
  config->history.bg=COLOR_GREEN;
  config->status.show=TRUE;
  config->status.fg=COLOR_GREEN;
  config->status.bg=COLOR_BLACK;
  config->info.show=TRUE;
  config->info.fg=COLOR_RED;
  config->info.bg=COLOR_BLACK;
  config->key.pause='p';
  config->key.stop='S';
  config->key.love='l';
  config->key.next='n';
  config->key.ban='B';
  config->key.radio='r';
  config->key.discovery='d';
  config->key.quit='Q';

  fd=fopen(configfile,"r");

  /* config to read */
  if (fd == NULL)
    return;

  while(fgets(line,sizeof(line),fd) != NULL)
  {
    if(line[0]=='#')
      continue;

    key=strtok_r(line,"=",&saveptr);
    if(key==NULL || key[0]==0xa)
      continue;
    trim(key);
    keyval=strtok_r(NULL,"=",&saveptr);
    if(keyval==NULL || key[0]==0xa)
      continue;
    trim(keyval);

    if(!strcmp(key,"net.host")) {
      strncpy(config->net.host,keyval,strlen(config->net.host));
    } else if(!strcmp(key,"net.port")) {
      config->net.port=atoi(keyval);
    } else if(!strcmp(key,"history.show")) {
      config->history.show=atoi(keyval);
    } else if(!strcmp(key,"history.fg")) {
      config->history.fg=atoi(keyval);
    } else if(!strcmp(key,"history.bg")) {
      config->history.bg=atoi(keyval);
    } else if(!strcmp(key,"status.show")) {
      config->status.show=atoi(keyval);
    } else if(!strcmp(key,"status.fg")) {
      config->status.fg=atoi(keyval);
    } else if(!strcmp(key,"status.bg")) {
      config->status.bg=atoi(keyval);
    } else if(!strcmp(key,"info.show")) {
      config->info.show=atoi(keyval);
    } else if(!strcmp(key,"info.fg")) {
      config->info.fg=atoi(keyval);
    } else if(!strcmp(key,"info.bg")) {
      config->info.bg=atoi(keyval);
    } else if(!strcmp(key,"key.pause")) {
      config->key.pause=keyval[0];
    } else if(!strcmp(key,"key.stop")) {
      config->key.stop=keyval[0];
    } else if(!strcmp(key,"key.love")) {
      config->key.love=keyval[0];
    } else if(!strcmp(key,"key.next")) {
      config->key.next=keyval[0];
    } else if(!strcmp(key,"key.ban")) {
      config->key.ban=keyval[0];
    } else if(!strcmp(key,"key.radio")) {
      config->key.radio=keyval[0];
    } else if(!strcmp(key,"key.discovery")) {
      config->key.discovery=keyval[0];
    } else if(!strcmp(key,"key.quit")) {
      config->key.quit=keyval[0];
    }
  }
  fclose(fd);
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
  struct config config;
  int key=0;  // just pressed key for main loop
  char tmp[512];

  atexit(quit);

  read_config(&config);


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

  history = newwin(LINES-(4+config.info.show), COLS, 0, 0);
  status = newwin(4, COLS, LINES-(4+config.info.show), 0);
  scrollok(history, TRUE);
  info = newwin(1, COLS, LINES-1, 0);
  
  bkgd(COLOR_PAIR(1));
  wbkgd(status, COLOR_PAIR(2));
  wbkgd(history, COLOR_PAIR(3));
  wbkgd(info, COLOR_PAIR(4));

  snprintf(tmp,sizeof(tmp),"%c: pause - %c: stop - %c: love - %c: next - %c: ban "
    "- %c: change radio - %c: discovery - %c: quit"
    ,config.key.pause,config.key.stop,config.key.love
    ,config.key.next,config.key.ban,config.key.radio
    ,config.key.discovery,config.key.quit);
  mvwaddstr(info, 0, 0, tmp);

  refresh();
  wrefresh(status);
  wrefresh(history);
  if(config.info.show)
    wrefresh(info);

  while((key=getch()) != config.key.quit)
  {
    keypresshandler(key);
  } 

  return(0);
}

