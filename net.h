#ifndef GANGBANG_NET_H
#define GANGBANG_NET_H

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "config.h"

int socket_connect(char *host, in_port_t port);
int send_command(char *command);

#endif /* GANGBANG_NET_H */
