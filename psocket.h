#ifndef PSOCKET_H
#define PSOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/** Get sockaddr, IPv4 or IPv6. */
void *get_in_addr(struct sockaddr *sa);

/** Create a new socket. */
int create_socket(const char* port);

#ifdef __cplusplus
}
#endif

#endif
