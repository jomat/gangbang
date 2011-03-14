/*
	Copyright (C) 2011 jomat <jomat+gangbang@jmt.gr>
	Published under the terms of the GNU General Public License (GPL).
*/

#include "gangbang.h"

void quit(void)
{
  del_panel(pstatus);
  del_panel(phistory);
  delwin(status);
  delwin(history);
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
  refresh_main_screen();
}

void refresh_main_screen()
{
  char tmp[1024];
  PANEL *pan = panel_above((PANEL *)0);
  
  pnoutrefresh(history,0,0,0,0,LINES - (4 + config.info.show),COLS);
  do { /* necessary because panel lib doesn't know about prefesh() et al */
    touchwin(panel_window(pan));
  } while ((pan=panel_above(pan)));
  if (config.info.show)
  {
     snprintf(tmp, sizeof(tmp),
              "%c: pause - %c: stop - %c: love - %c: next - %c: ban "
              "- %c: change radio - %c: discovery - %c: quit",
              config.key.pause, config.key.stop, config.key.love,
              config.key.next, config.key.ban, config.key.radio,
              config.key.discovery, config.key.quit);
    mvwaddstr(stdscr, LINES-1, 0, tmp);
  }
  update_panels();
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
  mvwin(status,LINES - (4 + config.info.show),0);
  oldLINES=LINES;
  refresh_main_screen();
}

int show_input_dialog(char *title,char *input,bool input_clear)
{
  FIELD *field[2];
  FORM  *my_form;
  WINDOW *my_form_win;
  PANEL *my_panel;
  int ch, rows, cols;

  field[0] = new_field(1, strlen(input), 1, 1, 0, 0);
  field[1] = NULL;

  set_field_opts(field[0]
    ,O_VISIBLE
    |O_ACTIVE
    |O_PUBLIC
    |O_EDIT
//    |O_WRAP
    //|(input_clear?O_BLANK:!O_BLANK)  // TODO: 
//    |O_BLANK
//    |O_AUTOSKIP
    |O_NULLOK
//    |O_STATIC
    |O_PASSOK);

  set_max_field(field[0], 256);

  set_field_buffer(field[0],0,input);

  my_form = new_form(field);

  scale_form(my_form, &rows, &cols);

  my_form_win = newwin(rows + 1, cols + 3, 4, 4);
  my_panel = new_panel(my_form_win);
  keypad(my_form_win, TRUE);

  set_form_win(my_form, my_form_win);
  set_form_sub(my_form, derwin(my_form_win, rows, cols, 0, 1));
  curs_set(1);

  box(my_form_win, 0, 0);

  post_form(my_form);
  box(my_form_win, 0, 0);

  for(int i=0;input[i]&&input[i++]!='$';form_driver(my_form, REQ_NEXT_CHAR));

  while((ch = wgetch(my_form_win)) != 0xA) {
      debug("key %x\n",ch);
    switch(ch) {
      case KEY_RESIZE:
        form_driver(my_form,REQ_VALIDATION);
        strncpy(input,field_buffer(field[0],0),512);  /* TODO: */
        window_size_changed();
        return -1;
        break;
      case KEY_RIGHT:
        form_driver(my_form, REQ_NEXT_CHAR);
        break;
      case KEY_LEFT:
        form_driver(my_form, REQ_PREV_CHAR);
        break;
      case 0x7f:  /* KEY_BACKSPACE didn't seem to work */
      case KEY_BACKSPACE:
        form_driver(my_form, REQ_DEL_PREV);
        break;
      case KEY_DL:
      case 0x14a: /* KEY_DL didn't seem to work */
        form_driver(my_form, REQ_DEL_CHAR);
        break;
      default:
        form_driver(my_form, ch);
        break;
    }
  }
  /* something like this is needed to sync the actual buffer.
   * http://docsrv.sco.com/SDK_charm/_Setting_and_Reading_Field_Buffe.html writes:
    The function field_buffer always returns the correct value if the field is not current.
    However, if the field is current, the function is sometimes inaccurate because data is
    not moved to field buffer 0 immediately upon entry. You may rest assured that
    field_buffer is accurate on the current field if
    * it is called from the field check validation routine, if any
    * it is called from the form or field initialization or termination routines, if any
    * it is called just after a REQ_VALIDATION request to the form driver 
   */
  form_driver(my_form,REQ_VALIDATION);
  strncpy(input,field_buffer(field[0],0),512);  /* TODO: */
  trim(input);

  curs_set(0);
  unpost_form(my_form);
  free_form(my_form);
  free_field(field[0]);
  free_field(field[1]); 

  delwin(my_form_win);
  del_panel(my_panel);

  return 1;
}

int show_menu(char *choices[],int n_choices,char *title,int selection)
{
  int i,rows,columns,it_idx;
  ITEM **it;
  MENU *me;
  WINDOW *win,*dwin;
  PANEL *pan;
  int ch;

  it = (ITEM **)calloc(n_choices, sizeof(ITEM *));
  for(i=0; i<n_choices; i++) {
    if(!(it[i]=new_item(choices[i],"")) && i!=n_choices-1 )
      exit(1);  // TODO: how about an error message
  }

  me = new_menu(it);

  scale_menu(me,&rows,&columns);

  win = newwin((LINES<rows+3)?LINES:rows+3, columns+3, 0, 0);
  pan = new_panel(win);
  top_panel(pan);
  keypad(win, TRUE);
  set_menu_win (me, win);
  set_menu_sub (me, dwin=derwin(win, LINES-3<rows?LINES-3:rows, columns, 2, 2));
  set_menu_format(me,LINES-3<rows?LINES-3:rows,1);
  box(win, 0, 0);  
  mvwaddstr(win, 1, 2, title);
  set_current_item(me,it[selection]);

  post_menu(me);  
  update_panels();
  doupdate();

  while((ch=getch()) != 0xA)
  {
    switch(ch)
    {
      case KEY_RESIZE:
        window_size_changed();
        return -1-item_index(current_item(me));;
        break;
      case KEY_DOWN:
        menu_driver(me, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(me, REQ_UP_ITEM);
        break;
    }
    update_panels();
    doupdate();
  } 

  it_idx = item_index(current_item(me));

  unpost_menu(me);
  free_menu(me);

  for(i=0; i<n_choices; i++)
    free_item(it[i]);

  free(it);
  del_panel(pan);
  delwin(win);
  refresh_main_screen();
  return it_idx;
}

void keypresshandler(int key)
{
  char tmp[512];
  char *stations[] = {
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
  int choice=-1;
  char input[512];

  if (key == 'c')
    update_status();

  /* TODO: check return value of send_command()s */
  if (KEY_RESIZE == key) {
    window_size_changed();
  } else if (config.key.pause == key) {
    addhistory("trying to toggle pause");
    send_command("pause",5);
  } else if (config.key.stop == key) {
    addhistory("trying to stop");
    send_command("stop",4);
  } else if (config.key.love == key) {
    addhistory("trying to love current track");
    send_command("love",4);
  } else if (config.key.next == key) {
    addhistory("trying to skip");
    send_command("skip",4);
  } else if (config.key.ban == key) {
    addhistory("trying to ban");
    send_command("ban",3);
  } else if (config.key.radio == key) {
    int ret=0;
    while((choice=show_menu(stations,ARRAY_SIZE(stations),"change radio station",-1-choice))<0);
    if(!choice)
      return;
    strncpy(input,stations[choice]+9,sizeof(input));
    while((ret=show_input_dialog(NULL,input,ret?0:1))<0);
    snprintf(tmp,sizeof(tmp),"play lastfm://%s",input);
    send_command(tmp,strlen(tmp));
    snprintf(tmp,sizeof(tmp),"selected station: %s: %s-",stations[choice],input);
    addhistory(tmp);
  } else if (config.key.discovery == key) {
    addhistory("trying to toggle discovery");
    send_command("discovery",9);
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
  init_pair(1, config.status.fg, config.status.bg);
  init_pair(2, config.history.fg, config.history.bg);
  init_pair(3, config.info.fg, config.info.bg);

  history = newpad(LINES - (4 + config.info.show), COLS);
  status = newwin(4, COLS, LINES - (4 + config.info.show), 0);
  scrollok(history, TRUE);
  pstatus = new_panel(status);
  phistory = new_panel(history);

  wbkgd(status, COLOR_PAIR(1));
  wbkgd(history, COLOR_PAIR(2));
  wbkgd(stdscr, COLOR_PAIR(3));

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

