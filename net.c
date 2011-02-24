#include "net.h"

int socket_connect(char *host, in_port_t port)
{
  struct hostent *hp;
  struct sockaddr_in addr;
  int on = 1, sock;

  if ((hp = gethostbyname(host)) == NULL) {
    herror("gethostbyname");
    exit(1);
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

