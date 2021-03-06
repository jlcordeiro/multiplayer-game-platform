#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "tcpsocket.h"
#include <anet/anet.h>

int anetTcpConnect(char *err, const char *addr, int port);
int anetTcpNonBlockConnect(char *err, const char *addr, int port);
int anetTcpServer(char *err, int port, const char *bindaddr);

int TCPServer::recv_connection()
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

static queue<string> split_string(const string& str)
{
    queue<string> strings;

    string::size_type pos = 0;
    string::size_type prev = 0;
    while ((pos = str.find("\n", prev)) != string::npos)
    {
        strings.push(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    string new_str = str.substr(prev);
    if (new_str.size() > 0) {
        strings.push(new_str);
    }

    return strings;
}

void TCPServer::recv_data(int socket)
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

        auto tokens = split_string(string(buf));
        if (auto pfn = _data_fn.lock()) {
            while (!tokens.empty()) {
                (*pfn)(socket, tokens.front());
                tokens.pop();
            }
        }
    }
}

TCPServer::TCPServer(const char* port)
    :   _listener(anetTcpServer(NULL, atoi(port), NULL)),
        _fdmax(_listener)
{
    if (_listener == -1) {
        throw _listener;
    }

    // add the _listener to the master set
    FD_ZERO(&_fds);
    FD_SET(_listener, &_fds);
}

int TCPServer::go()
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

void TCPServer::set_data_fn(shared_ptr<data_fn_t> f)
{
    _data_fn = f;
}

void TCPServer::set_connect_fn(shared_ptr<conn_fn_t> f)
{
    _connect_fn = f;
}

void TCPServer::set_disconnect_fn(shared_ptr<conn_fn_t> f)
{
    _disconnect_fn = f;
}

// -----

TCPClient::TCPClient(const char* host, int port, bool block = true)
    : _block(block)
{
    if (block) {
        _fd = anetTcpConnect(NULL, host, port);
    } else {
        _fd = anetTcpNonBlockConnect(NULL, host, port);
    }

    if (_fd < 0) {
        throw _fd;
    }
}

int TCPClient::recv(queue<string>& recv_messages)
{
    char buffer[BUF_SIZE];
    bzero(buffer, BUF_SIZE);

    auto n = read(_fd, buffer, BUF_SIZE);
    if (n < 0) {
        if (errno == EINTR || (errno == EAGAIN && !_block)) {
            return 0;
        }
        return -1;
    }

    if (n == 0) {
        return -1;
    }

    recv_messages = split_string(string(buffer));

    return 0;
}

// ----

int send(int fd, const string& msg)
{
    return send(fd, string(msg+"\r\n").c_str(), msg.length()+2, 0);
}
