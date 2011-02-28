/*
	Copyright (C) 2011 jomat <jomat+gangbang@jmt.gr>
	Published under the terms of the GNU General Public License (GPL).
*/

#include "updatestatus.h"
#include "config.h"

void tokenize_songinfo(char *buf,struct songinfo *songinfo)
{
  char *saveptr = 0
    , *token;

  token=strtok_r(buf, "|", &saveptr);
  token[strlen(token)-1]=0;
  strncpy(songinfo->artist,token,sizeof(songinfo->artist));
  token=strtok_r(NULL, "|", &saveptr);
  token[strlen(token)-1]=0;
  strncpy(songinfo->track,token,sizeof(songinfo->track));
  token=strtok_r(NULL, "|", &saveptr);
  token[strlen(token)-1]=0;
  strncpy(songinfo->album,token,sizeof(songinfo->album));
  token=strtok_r(NULL, "|", &saveptr);
  token[strlen(token)-1]=0;
  songinfo->duration=atoi(token);
  token=strtok_r(NULL, "|", &saveptr);
  token[strlen(token)-1]=0;
  strncpy(songinfo->station,token,sizeof(songinfo->station));
  token=strtok_r(NULL, "|", &saveptr);
  token[strlen(token)-1]=0;
  strncpy(songinfo->stationurl,token,sizeof(songinfo->stationurl));
  token=strtok_r(NULL, "|", &saveptr);
  token[strlen(token)-1]=0;
  strncpy(songinfo->artisturl,token,sizeof(songinfo->artisturl));
  token=strtok_r(NULL, "|", &saveptr);
  token[strlen(token)-1]=0;
  strncpy(songinfo->albumurl,token,sizeof(songinfo->albumurl));
  token=strtok_r(NULL, "|", &saveptr);
  token[strlen(token)-1]=0;
  strncpy(songinfo->trackurl,token,sizeof(songinfo->trackurl));
  token=strtok_r(NULL, "|", &saveptr);
  token[strlen(token)-1]=0;
  songinfo->remaining=atoi(token);
}

void update_status()
{
  int n,lineno=0;
  int sock_status;
  char buf[4096];
  char line[COLS];
  struct songinfo songinfo;
  static char currtrack[256],currartist[256];

  if((sock_status = socket_connect(config.net.host, config.net.port))<0) {
    mvwaddstr(status, 0, 0, "can't connect to remote");
    wrefresh(status);
    return;
  }
  n = write(sock_status, INFOREQUEST, sizeof(INFOREQUEST));
  if (n < 0) {
    mvwaddstr(status, 0, 0, "cant write to remote");
    wrefresh(status);
  } else {
    n=read(sock_status, buf, sizeof(buf) - 1);
    if(n<1)
      return;
    tokenize_songinfo(buf,&songinfo);
    snprintf(line,COLS,"now playing: %s - %s (%i/%i)\n"
      ,songinfo.artist,songinfo.track,songinfo.duration-songinfo.remaining
      ,songinfo.duration);
    mvwaddstr(status, lineno++, 0, line);
    snprintf(line,COLS,"Station: %s",songinfo.station);
    mvwaddstr(status, lineno++, 0, line);

    if(strcmp(songinfo.track,currtrack)||strcmp(songinfo.artist,currartist))
    {
      if(currtrack[0]|currartist[0]) {
        snprintf(line,COLS,"%s - %s",currartist,currtrack);
        addhistory(line);
      }
      strncpy(currartist,songinfo.artist,sizeof(currartist));
      strncpy(currtrack,songinfo.track,sizeof(currtrack));
    }
    
  }
  close(sock_status);

  wrefresh(status);
}

void *update_status_loop()
{
  int snooze;
  while(1) {
    update_status();
    do {
      snooze=sleep(config.lnf.statusupdate);
    } while(snooze);
  }
}

