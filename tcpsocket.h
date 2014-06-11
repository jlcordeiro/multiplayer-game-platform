#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>
#include "psocket.h"

/** \brief Handler interface that implements the
 * handling of each event that can occur on the socket.
 */
class TCPSocketHandler {
    public:
        virtual void handle_connect(int fd) = 0;
        virtual void handle_disconnect(int fd) = 0;
        virtual void handle_data(int fd, const char* what, size_t nbytes) = 0;
        virtual void handle_send_error(int fd, const char* what, size_t nbytes) = 0;
        virtual void handle_select_error() = 0;
};

class TCPSocket {
    private:
        TCPSocket();

        /** \brief Handler for the different event that can occur. */
        TCPSocketHandler*   _handler;
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

        /**
         * \brief Send a message to all the file descriptors.
         */
        void broadcast(const char* buf, size_t nbytes) const;

        /**
         * \brief Send a message to all the file descriptors.
         */
        void broadcast(const std::string& msg) const;

    public:
        enum {BUF_SIZE = 1024};

        TCPSocket(const char* port, TCPSocketHandler* handler);

        /**
         * \brief Check all the file descriptors and act accordingly.
         */
        int go();
};

#endif
