#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <map>
#include <string>
#include <iostream>
#include "psocket.h"
using namespace std;

class TCPSocket {
    private:
        TCPSocket();

        /** \brief Listening socket descriptor. */
        int                 _listener;
        /** \brief File descriptor list. */
        fd_set              _fds;
        /** \brief Max file descriptor number. */
        int                 _fdmax;
        /** \brief Flag that shows if the server is running. */
        bool                _running;

        /**
         * \brief Handle a new incoming connection.
         * \return -1 on error. 0 otherwise.
         */
        int recv_connection();

        /**
         * \brief Handle the data received from a socket.
         */
        void recv_data(int socket);

    protected:
        virtual void handle_connect(int fd) = 0;
        virtual void handle_disconnect(int fd) = 0;
        virtual void handle_data(int fd, const char* what, size_t nbytes) = 0;
        virtual void handle_select_error() = 0;

    public:
        enum {BUF_SIZE = 1024};

        TCPSocket(const char* port);

        /**
         * \brief Check all the file descriptors and act accordingly.
         */
        int go();
};

int send(int fd, const string& msg);

#endif
