#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "server.h"

// get sockaddr, IPv4 or IPv6:
static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static int create_socket(const char* port)
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

int TCPServer::handle_new_connection()
{
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen = sizeof remoteaddr;

    int newfd = accept(_listener, (struct sockaddr *)&remoteaddr, &addrlen);

    if (newfd == -1) {
        return -1;
    } else {
        FD_SET(newfd, &_fds); // add to master set
        if (newfd > _fdmax) {    // keep track of the max
            _fdmax = newfd;
        }

        char remoteIP[INET6_ADDRSTRLEN];
        printf("server: new connection from %s on socket %d\n",
                inet_ntop(remoteaddr.ss_family,
                    get_in_addr((struct sockaddr*)&remoteaddr),
                    remoteIP, INET6_ADDRSTRLEN),
                newfd);
    }

    return 0;
}

int TCPServer::broadcast(const char* buf, size_t nbytes) const
{
    int failed = 0;

    for(int j = 0; j <= _fdmax; j++) {
        if (FD_ISSET(j, &_fds) && j != _listener) {
            if (send(j, buf, nbytes, 0) == -1) {
                failed += 1;
            }
        }
    }

    return failed;
}

int TCPServer::broadcast(const std::string& msg) const
{
    return broadcast(msg.c_str(), msg.length());
}

void TCPServer::handle_recv_data(int socket)
{
    char buf[BUF_SIZE];
    size_t nbytes;
    if ((nbytes = recv(socket, buf, sizeof buf, 0)) <= 0) {
        printf("server: socket %d closed.\n", socket);
        close(socket);
        FD_CLR(socket, &_fds); // remove from master set
    } else {
        if (broadcast(buf, nbytes)) {
            perror("broadcast failed.");
        }
    }
}

TCPServer::TCPServer(const char* port)
    :   _listener(create_socket(port)),
        _fdmax(_listener),
        _running(_listener != -1)
{
    // add the _listener to the master set
    FD_ZERO(&_fds);
    FD_SET(_listener, &_fds);
}

int TCPServer::go()
{
    fd_set read_fds; // temp file descriptor list for select()
    FD_ZERO(&read_fds);
    read_fds = _fds;

    if (select(_fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
        return 0;
    }

    int i;
    for(i = 0; i <= _fdmax; i++) {
        if (FD_ISSET(i, &read_fds)) {
            if (i == _listener) {
                if (handle_new_connection()) {
                    return -1;
                }
            } else {
                handle_recv_data(i);
            }
        }
    }

    return 0;
}
