#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <map>
#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#ifndef __APPLE__
#include <memory>
#include <strings.h>
#include <functional>
#endif
#include "psocket.h"
using namespace std;

typedef function<void(int)> conn_fn_t;
typedef shared_ptr<function<void(int)>> sp_conn_fn_t;
typedef function<void(int, const string&)> data_fn_t;
typedef shared_ptr<function<void(int, const string&)>> sp_data_fn_t;

enum {BUF_SIZE = 2048};

class TCPServer {
    private:
        TCPServer();

        /** \brief Listening socket descriptor. */
        int                 _listener;
        /** \brief File descriptor list. */
        fd_set              _fds;
        /** \brief Max file descriptor number. */
        int                 _fdmax;

        /**
         * \brief Handle a new incoming connection.
         * \return -1 on error. 0 otherwise.
         */
        int recv_connection();

        /**
         * \brief Handle the data received from a socket.
         */
        void recv_data(int socket);

        /**
         * \brief Function that will be called when new data is available.
         */
        weak_ptr<data_fn_t> _data_fn;

        /**
         * \brief Function that will be called when a new connection is received.
         */
        weak_ptr<conn_fn_t> _connect_fn;

        /**
         * \brief Function that will be called when someone disconnects.
         */
        weak_ptr<conn_fn_t> _disconnect_fn;

    public:
        TCPServer(const char* port);

        /**
         * \brief Check all the file descriptors and act accordingly.
         */
        int go();

        /**
         * \brief Set the function to be called on new data.
         */
        void set_data_fn(shared_ptr<data_fn_t> f);

        /**
         * \brief Set the function to be called on new connection.
         */
        void set_connect_fn(shared_ptr<conn_fn_t> f);

        /*
         * \brief Set the function to be called on disconnection.
         */
        void set_disconnect_fn(shared_ptr<conn_fn_t> f);
};

int send(int fd, const string& msg);

class TCPClient {
private:
    int _fd;
    const bool _block;

public:
    TCPClient(const char* host, int port, bool block = true)
        : _fd(connect_to_socket(host, port)),
          _block(block)
    {
        if (_fd < 0) {
            throw _fd;
        }

        if (!_block) {
            fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
        }
    }

    ~TCPClient()
    {
        close(_fd);
    }

    int send(const string& msg) const
    {
        return ::send(_fd, msg);
    }

    /**
     * \brief Read from the socket, creating a vector with the strings read.
     * \return  -1 if the socket had an error or was closed.
     *          0 if nothing was read but socket is still open.
     *          >0 if something was read. This value is the number of characters read.
     */
    int recv(vector<string>& recv_messages);
};

#endif
