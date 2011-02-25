#include "gangbang.h"


void quit(void)
{
  delwin(status);
  delwin(history);
  delwin(info);
  endwin();
}

void addhistory(char *line)
{
  int histsizey, histsizex;
  char tmp[COLS];
  char timestr[6];
  time_t curtime;
  struct tm *loctime;

  curtime = time(NULL);
  loctime = localtime(&curtime);
  strftime(timestr, 6, "%H:%M ", loctime);

  getmaxyx(history, histsizey, histsizex);

  scroll(history);

  snprintf(tmp,COLS,"%s %s",timestr,line);

  mvwinsstr(history, histsizey - 1, 0, tmp);
  wrefresh(history);
}

void keypresshandler(int key)
{
  if (key == 'c')
    update_status();

  /* TODO: check return value of send_command()s */
  if (config.key.pause == key) {
    addhistory("trying to toggle pause");
    send_command("pause");
  } else if (config.key.stop == key) {
    addhistory("trying to stop");
    send_command("stop");
  } else if (config.key.love == key) {
    addhistory("trying to love current track");
    send_command("love");
  } else if (config.key.next == key) {
    addhistory("trying to skip");
    send_command("skip");
  } else if (config.key.ban == key) {
    addhistory("trying to ban");
    send_command("ban");
  } else if (config.key.radio == key) {
    addhistory("changing radio station not yet implemented :B");
  } else if (config.key.discovery == key) {
    addhistory("trying to toggle discovery");
    send_command("discovery");
  }
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
  pthread_t status_thread;

  atexit(quit);

  read_config();

  create_windows();

  pthread_create(&status_thread, NULL, update_status_loop, NULL);

  mainloop();

  return (0);
}

