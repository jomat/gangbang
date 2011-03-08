/*
	Copyright (C) 2011 jomat <jomat+gangbang@jmt.gr>
	Published under the terms of the GNU General Public License (GPL).
*/

#include "net.h"

int socket_connect(char *host, in_port_t port)
{
  struct hostent *hp;
  struct sockaddr_in addr;
  int on = 1, sock;

  if ((hp = gethostbyname(host)) == NULL) {
    // TODO: add some logging facility for these errors
    return -1;
  }
  bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *) &on,
             sizeof(int));

  if (sock == -1)
    return -1;

  if (connect(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in))
      == -1) {
    close(sock);
    return -1;
  }

  return sock;
}

int send_command(char *command,int bytes)
{
  int socket,n;

  socket=socket_connect(config.net.host, config.net.port);

  n=write(socket, command, bytes);

  close(socket);

  return n;
}
