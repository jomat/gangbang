#include "updatestatus.h"
#include "config.h"

void update_status()
{
  int n;
  int sock_status;
  char buf[1024];
  sock_status = socket_connect(config.net.host, config.net.port);
  n = write(sock_status, "info %a|%t|%A|%d|%s|%u|%U|%X|%T|%R\n", 36);
  if (n < 0) {
    mvwaddstr(status, 0, 0, "cant write to remote");
    wrefresh(status);
  } else {
    read(sock_status, buf, sizeof(buf) - 1);
    mvwaddstr(status, 0, 0, buf);
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

