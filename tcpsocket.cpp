#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tcpsocket.h"

int TCPSocket::handle_new_connection()
{
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen = sizeof remoteaddr;

    int newfd = accept(_listener, (struct sockaddr *)&remoteaddr, &addrlen);

    if (newfd >= 0) {
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

    return newfd;
}

int TCPSocket::broadcast(const char* buf, size_t nbytes) const
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

int TCPSocket::broadcast(const std::string& msg) const
{
    return broadcast(msg.c_str(), msg.length());
}

void TCPSocket::handle_recv_data(int socket)
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

TCPSocket::TCPSocket(const char* port)
    :   _listener(create_socket(port)),
        _fdmax(_listener),
        _running(_listener != -1)
{
    // add the _listener to the master set
    FD_ZERO(&_fds);
    FD_SET(_listener, &_fds);
}

int TCPSocket::go()
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
