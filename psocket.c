#include "psocket.h"
#include <unistd.h>
#include <string.h>

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int create_socket(const char* port)
{
    int sd;
    struct addrinfo hints, *ai, *p;

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
        return -1;
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sd < 0) { 
            continue;
        }

        // no "address already in use" error message
        int yes=1;
        setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(sd, p->ai_addr, p->ai_addrlen) < 0) {
            close(sd);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(ai);

    if (listen(sd, 10) == -1) {
        return -1;
    }

    return sd;
}

int connect_to_socket(const char* host, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return -1;
    }

    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        return -1;
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        return -1;
    }

    return sockfd;
}
