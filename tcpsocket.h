#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <map>
#include <string>
#include <iostream>
#include "user.h"
#include "room.h"
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
        /**
         * \brief Send a message to all the file descriptors.
         */
        void broadcast(const char* buf, size_t nbytes) const;

        /**
         * \brief Send a message to all the file descriptors.
         */
        void broadcast(const std::string& msg) const;

        virtual void handle_connect(int fd) = 0;
        virtual void handle_disconnect(int fd) = 0;
        virtual void handle_data(int fd, const char* what, size_t nbytes) = 0;
        virtual void handle_send_error(int fd, const char* what, size_t nbytes) const = 0;
        virtual void handle_select_error() = 0;

    public:
        enum {BUF_SIZE = 1024};

        TCPSocket(const char* port);

        /**
         * \brief Check all the file descriptors and act accordingly.
         */
        int go();
};

typedef function<void(shared_ptr<Room>, const string&)> room_data_fn_t;
typedef function<void(shared_ptr<User>, const string&)> user_data_fn_t;

class RoomSocket : public TCPSocket
{
private:
    map<int,shared_ptr<Room> >& _rooms;
    weak_ptr<room_data_fn_t> _server_data_fn;

public:
    RoomSocket(const char* port, map<int,shared_ptr<Room> >& rooms)
        : TCPSocket(port),
          _rooms(rooms)
    {
    }

    void handle_connect(int fd)
    {
        cout << "Adding room [" << fd << "]." << endl;
        _rooms[fd] = shared_ptr<Room>(new Room(fd));
    }

    void handle_disconnect(int fd)
    {
        cout << "Removing room [" << fd << "]." << endl;
        _rooms.erase(fd);
    }

    void handle_data(int fd, const char* what, size_t nbytes)
    {
        if (auto pfn = _server_data_fn.lock()) {
            (*pfn)(_rooms[fd], string(what));
        }
    }

    void handle_send_error(int fd, const char* what, size_t nbytes) const
    {
        cout << "Failed to send " << what << " to [" << fd << "]." << endl;
    }

    void handle_select_error()
    {
    }

    void set_process_data_fn(shared_ptr<room_data_fn_t> f)
    {
        _server_data_fn = f;
    }
};

class UserSocket : public TCPSocket
{
private:
    map<int,shared_ptr<User> >& _users;
    weak_ptr<user_data_fn_t> _server_data_fn;

public:
    UserSocket(const char* port, map<int,shared_ptr<User> >& users)
        : TCPSocket(port),
          _users(users)
    {
    }

    void handle_connect(int fd)
    {
        cout << "Adding user [" << fd << "]." << endl;
        _users[fd] = shared_ptr<User>(new User(fd));
    }

    void handle_disconnect(int fd)
    {
        cout << "Removing user [" << fd << "]." << endl;
        _users.erase(fd);
    }

    void handle_data(int fd, const char* what, size_t nbytes)
    {
        if (auto pfn = _server_data_fn.lock()) {
            (*pfn)(_users[fd], string(what));
        }
    }

    void handle_send_error(int fd, const char* what, size_t nbytes) const
    {
        cout << "Failed to send " << what << " to user [" << fd << "]." << endl;
    }

    void handle_select_error()
    {
    }

    void set_process_data_fn(shared_ptr<user_data_fn_t> f)
    {
        _server_data_fn = f;
    }
};

#endif
