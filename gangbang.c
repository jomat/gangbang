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

  if (config.key.pause == key) {
      addhistory("trying to pause");
      send_command("pause");
  } else if (config.key.stop == key) {
  } else if (config.key.love == key) {
  } else if (config.key.next == key) {
  } else if (config.key.ban == key) {
  } else if (config.key.radio == key) {
  } else if (config.key.discovery == key) {
  } else if (config.key.quit == key) {
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

