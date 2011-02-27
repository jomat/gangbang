/*
	Copyright (C) 2011 jomat <jomat+gangbang@jmt.gr>
	Published under the terms of the GNU General Public License (GPL).
*/

#ifndef GANGBANG_UPDATESTATUS_H
#define GANGBANG_UPDATESTATUS_H

#include "net.h"
#include "gangbang.h"

#define INFOREQUEST "info %a |%t |%l |%d |%s |%S |%A |%L |%T |%R |\n"

struct songinfo {
  char artist[256]
    , track[256]
    , album[256]
    , station[256]
    , stationurl[512]
    , artisturl[512]
    , albumurl[512]
    , trackurl[512];
  int duration
    , remaining;
};

void tokenize_songinfo(char *buf,struct songinfo *songinfo);
void update_status();
void *update_status_loop();

#endif /* GANGBANG_UPDATESTATUS_H */
