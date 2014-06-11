#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "psocket.h"

class TCPSocket {
    private:
        TCPSocket();

        /** \brief Listening socket descriptor. */
        int     _listener;
        /** \brief File descriptor list. */
        fd_set  _fds;
        /** \brief Max file descriptor number. */
        int     _fdmax;
        /** \brief Flag that shows if the server is running. */
        bool    _running;

        /**
         * \brief Handle a new incoming connection.
         * \return -1 on error. 0 otherwise.
         */
        int handle_new_connection();

        /**
         * \brief Handle the data received from a socket.
         */
        void handle_recv_data(int socket);

        /**
         * \brief Send a message to all the file descriptors.
         */
        int broadcast(const char* buf, size_t nbytes) const;

        /**
         * \brief Send a message to all the file descriptors.
         */
        int broadcast(const std::string& msg) const;

    public:
        enum {BUF_SIZE = 1024};

        TCPSocket(const char* port);

        /**
         * \brief Check all the file descriptors and act accordingly.
         */
        int go();
};

#endif
