#include "config.h"

void trim(char *s)
{
  char *p = s;
  int l = strlen(p);

  while (isspace(p[l - 1]))
    p[--l] = 0;
  while (*p && isspace(*p))
    ++p, --l;

  memmove(s, p, l + 1);
}

void read_config()
{
  FILE *fd;
  char line[512];
  char configfile[FILENAME_MAX];
  char *key, *keyval, *saveptr = 0;
  snprintf(configfile, FILENAME_MAX, "%s/%s", getenv("HOME"),
           ".gangbangrc");

  strcpy(config.net.host, "schwester.club.muc.ccc.de");
  config.net.port = 54311;
  config.history.show = TRUE;
  config.history.fg = COLOR_BLACK;
  config.history.bg = COLOR_GREEN;
  config.status.show = TRUE;
  config.status.fg = COLOR_GREEN;
  config.status.bg = COLOR_BLACK;
  config.info.show = TRUE;
  config.info.fg = COLOR_RED;
  config.info.bg = COLOR_BLACK;
  config.key.pause = 'p';
  config.key.stop = 'S';
  config.key.love = 'l';
  config.key.next = 'n';
  config.key.ban = 'B';
  config.key.radio = 'r';
  config.key.discovery = 'd';
  config.key.quit = 'Q';

  fd = fopen(configfile, "r");

  /* config to read */
  if (fd == NULL)
    return;

  while (fgets(line, sizeof(line), fd) != NULL) {
    if (line[0] == '#')
      continue;

    key = strtok_r(line, "=", &saveptr);
    if (key == NULL || key[0] == 0xa)
      continue;
    trim(key);
    keyval = strtok_r(NULL, "=", &saveptr);
    if (keyval == NULL || key[0] == 0xa)
      continue;
    trim(keyval);

    if (!strcmp(key, "net.host")) {
      strncpy(config.net.host, keyval, strlen(config.net.host));
    } else if (!strcmp(key, "net.port")) {
      config.net.port = atoi(keyval);
    } else if (!strcmp(key, "history.show")) {
      config.history.show = atoi(keyval);
    } else if (!strcmp(key, "history.fg")) {
      config.history.fg = atoi(keyval);
    } else if (!strcmp(key, "history.bg")) {
      config.history.bg = atoi(keyval);
    } else if (!strcmp(key, "status.show")) {
      config.status.show = atoi(keyval);
    } else if (!strcmp(key, "status.fg")) {
      config.status.fg = atoi(keyval);
    } else if (!strcmp(key, "status.bg")) {
      config.status.bg = atoi(keyval);
    } else if (!strcmp(key, "info.show")) {
      config.info.show = atoi(keyval);
    } else if (!strcmp(key, "info.fg")) {
      config.info.fg = atoi(keyval);
    } else if (!strcmp(key, "info.bg")) {
      config.info.bg = atoi(keyval);
    } else if (!strcmp(key, "key.pause")) {
      config.key.pause = keyval[0];
    } else if (!strcmp(key, "key.stop")) {
      config.key.stop = keyval[0];
    } else if (!strcmp(key, "key.love")) {
      config.key.love = keyval[0];
    } else if (!strcmp(key, "key.next")) {
      config.key.next = keyval[0];
    } else if (!strcmp(key, "key.ban")) {
      config.key.ban = keyval[0];
    } else if (!strcmp(key, "key.radio")) {
      config.key.radio = keyval[0];
    } else if (!strcmp(key, "key.discovery")) {
      config.key.discovery = keyval[0];
    } else if (!strcmp(key, "key.quit")) {
      config.key.quit = keyval[0];
    }
  }
  fclose(fd);
}

