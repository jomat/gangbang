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

  sock_status = socket_connect(config.net.host, config.net.port);
  n = write(sock_status, "info %a |%t |%l |%d |%s |%S |%A |%L |%T |%R \n", 46);
  if (n < 0) {
    mvwaddstr(status, 0, 0, "cant write to remote");
    wrefresh(status);
  } else {
    read(sock_status, buf, sizeof(buf) - 1);
    tokenize_songinfo(buf,&songinfo);
    snprintf(line,sizeof(line),"now playing: %s - %s (%i/%i)\n"
      ,songinfo.artist,songinfo.track,songinfo.duration-songinfo.remaining
      ,songinfo.duration);
    mvwaddstr(status, lineno++, 0, line);
    snprintf(line,sizeof(line),"Station: %s",songinfo.station);
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
  while(1) {
    update_status();
    sleep(config.lnf.statusupdate);
  }
}

