#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "tcpsocket.h"

int TCPSocket::recv_connection()
{
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen = sizeof remoteaddr;

    int newfd = accept(_listener, (struct sockaddr *)&remoteaddr, &addrlen);

    if (newfd >= 0) {
        FD_SET(newfd, &_fds); // add to master set
        if (newfd > _fdmax) {    // keep track of the max
            _fdmax = newfd;
        }

        if (auto pfn = _connect_fn.lock()) {
            (*pfn)(newfd);
        }
    }

    return newfd;
}

void TCPSocket::recv_data(int socket)
{
    char buf[BUF_SIZE];
    size_t nbytes;
    if ((nbytes = recv(socket, buf, sizeof buf, 0)) <= 0) {
        if (auto pfn = _disconnect_fn.lock()) {
            (*pfn)(socket);
        }

        close(socket);
        FD_CLR(socket, &_fds); // remove from master set
    } else {
        buf[nbytes-2] = '\0'; // remove \r\n

        if (auto pfn = _data_fn.lock()) {
            (*pfn)(socket, string(buf));
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

    struct timeval timeout;
    timeout.tv_sec = 0; 
    timeout.tv_usec = 10000; 
    
    int select_res = select(_fdmax+1, &read_fds, NULL, NULL, &timeout);
    if (select_res < 1) {
        if (select_res == -1) {
//             handle_select_error();
        }
        return select_res;
    }

    int i;
    for(i = 0; i <= _fdmax; i++) {
        if (FD_ISSET(i, &read_fds)) {
            if (i == _listener) {
                if (recv_connection()) {
                    return -1;
                }
            } else {
                recv_data(i);
            }
        }
    }

    return 0;
}

void TCPSocket::set_data_fn(shared_ptr<data_fn_t> f)
{
    _data_fn = f;
}

void TCPSocket::set_connect_fn(shared_ptr<conn_fn_t> f)
{
    _connect_fn = f;
}

void TCPSocket::set_disconnect_fn(shared_ptr<conn_fn_t> f)
{
    _disconnect_fn = f;
}

// ----

int send(int fd, const string& msg)
{
    return send(fd, msg.c_str(), msg.length(), 0);
}
