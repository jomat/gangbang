/*
	Copyright (C) 2011 jomat <jomat+gangbang@jmt.gr>
	Published under the terms of the GNU General Public License (GPL).
*/

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
  char tmp[COLS];
  char timestr[6];
  time_t curtime;
  struct tm *loctime;

  curtime = time(NULL);
  loctime = localtime(&curtime);
  strftime(timestr, 6, "%H:%M", loctime);

  scroll(history);

  snprintf(tmp,COLS,"%s %s",timestr,line);

  mvwinsstr(history, LINES - (5 + config.info.show), 0, tmp);
  prefresh(history,0,0,0,0,LINES - (4 + config.info.show),COLS);
}

void refresh_main_screen()
{
  char tmp[1024];
  touchwin(status);
  wnoutrefresh(stdscr);
  wnoutrefresh(status);
  pnoutrefresh(history,0,0,0,0,LINES - (4 + config.info.show),COLS);
  if (config.info.show)
  {
     snprintf(tmp, sizeof(tmp),
              "%c: pause - %c: stop - %c: love - %c: next - %c: ban "
              "- %c: change radio - %c: discovery - %c: quit",
              config.key.pause, config.key.stop, config.key.love,
              config.key.next, config.key.ban, config.key.radio,
              config.key.discovery, config.key.quit);
    mvwaddstr(info, 0, 0, tmp);
    wnoutrefresh(info);
  }
  doupdate();
}

void window_size_changed(void)
{
  // TODO: check redundant code in window initialization
  if(oldLINES-LINES > 0)
    wscrl(history, oldLINES-LINES);
  wresize(history,LINES - (4 + config.info.show),COLS);
  if (oldLINES-LINES < 0)
    wscrl(history, oldLINES-LINES);
  wresize(status,4,COLS);
  wresize(info,1,COLS);
  mvwin(status,LINES - (4 + config.info.show),0);
  mvwin(info,LINES - 1,0);
  oldLINES=LINES;
  refresh_main_screen();
}

int menu_change_radio(char *station)
{
  int i,rows,columns,n_choices,it_idx;
  ITEM **it;
  MENU *me;
  WINDOW *win,*dwin;
  int ch;
  char *choices[] = {
    "Cancel",
    "lastfm://user/$USER/loved",
    "lastfm://user/$USER/personal",
    "lastfm://usertags/$USER/$TAG",
    "lastfm://artist/$ARTIST/similarartists",
    "lastfm://globaltags/$TAG",
    "lastfm://user/$USER/recommended",
    "lastfm://user/$USER/playlist",
    "lastfm://tag/$TAG1*$TAG2*$TAG3",
    (char *)NULL,
  };
  n_choices = ARRAY_SIZE(choices);

  it = (ITEM **)calloc(n_choices, sizeof(ITEM *));
  for(i=0; i<n_choices; i++) {
    if(!(it[i]=new_item(choices[i],"")) && i!=n_choices-1 )
      exit(1);  // TODO: how about an error message
  }

  me = new_menu(it);

  scale_menu(me,&rows,&columns);

  win = newwin((LINES<rows+3)?LINES:rows+3, columns+3, 0, 0);
  keypad(win, TRUE);
  set_menu_win (me, win);
  set_menu_sub (me, dwin=derwin(win, LINES-3<rows?LINES-3:rows, columns, 2, 2));
  set_menu_format(me,LINES-3<rows?LINES-3:rows,1);
  box(win, 0, 0);  
  mvwaddstr(win, 1, 2, "Change radio station to:");

  post_menu(me);  
  wrefresh(win);


  while((ch=getch()) != 0xA)
  {
    switch(ch)
    {
      case KEY_RESIZE:
        window_size_changed();
        wresize(win,(LINES<rows+3)?LINES:rows+3, columns+3);
        wresize(dwin,LINES-3<rows?LINES-3:rows, columns);
        set_menu_format(me,LINES-3<rows?LINES-3:rows,1); 
        touchwin(win);
        break;
      case KEY_DOWN:
        menu_driver(me, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(me, REQ_UP_ITEM);
        break;
    }
    wrefresh(win);
  } 

  it_idx = item_index(current_item(me));

  unpost_menu(me);
  free_menu(me);

  for(i=0; i<n_choices; i++)
    free_item(it[i]);

  free(it);
  delwin(win);
  refresh_main_screen();
  return it_idx;
}

void keypresshandler(int key)
{
  char tmp[512];

  if (key == 'c')
    update_status();

  /* TODO: check return value of send_command()s */
  if (KEY_RESIZE == key) {
    window_size_changed();
  } else if (config.key.pause == key) {
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
    menu_change_radio(tmp);
  } else if (config.key.discovery == key) {
    addhistory("trying to toggle discovery");
    send_command("discovery");
  }
}

void create_windows()
{
  initscr();
  clear();
  noecho();
  curs_set(0);
  cbreak();
  keypad(stdscr, 1);

  start_color();
  use_default_colors();
  init_pair(2, config.status.fg, config.status.bg);
  init_pair(3, config.history.fg, config.history.bg);
  init_pair(4, config.info.fg, config.info.bg);

  history = newpad(LINES - (4 + config.info.show), COLS);
  status = newwin(4, COLS, LINES - (4 + config.info.show), 0);
  scrollok(history, TRUE);
  info = newwin(1, COLS, LINES - 1, 0);

  wbkgd(status, COLOR_PAIR(2));
  wbkgd(history, COLOR_PAIR(3));
  wbkgd(info, COLOR_PAIR(4));

  oldLINES=LINES;  // important to know for window resizing
  refresh_main_screen();
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

