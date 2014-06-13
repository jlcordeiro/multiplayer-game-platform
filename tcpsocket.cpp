#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "messages.h"
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

        handle_connect(newfd);
    }

    return newfd;
}

void TCPSocket::broadcast(const char* buf, size_t nbytes) const
{
    for(int j = 0; j <= _fdmax; j++) {
        if (FD_ISSET(j, &_fds) && j != _listener) {
            if (send(j, buf, nbytes, 0) == -1) {
                handle_send_error(j, buf, nbytes);
            }
        }
    }
}

void TCPSocket::broadcast(const std::string& msg) const
{
    broadcast(msg.c_str(), msg.length());
}

void TCPSocket::recv_data(int socket)
{
    char buf[BUF_SIZE];
    size_t nbytes;
    if ((nbytes = recv(socket, buf, sizeof buf, 0)) <= 0) {
        handle_disconnect(socket);
        close(socket);
        FD_CLR(socket, &_fds); // remove from master set
    } else {
        buf[nbytes-2] = '\0'; // remove \r\n
        handle_data(socket, buf, nbytes);
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
            handle_select_error();
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

void RoomSocket::handle_data(int fd, const char* what, size_t nbytes)
{
    cout << "Received " << what << " from [" << fd << "]." << endl;
    std::string err;
    if (isSetName(string(what), err)) {
        _rooms[fd]->setName(string(what));
    }
}

void UserSocket::handle_data(int fd, const char* what, size_t nbytes)
{
    cout << "Received " << what << " from [" << fd << "]." << endl;
        
    shared_ptr<User> user = _users[fd];;
    _server_data_fn(user, string(what));

    std::string err;
    if (isSetName(string(what), err)) {
        user->setName(string(what));
    }
}
