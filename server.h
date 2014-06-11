#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <iostream>
#include "room.h"
#include "user.h"
#include "tcpsocket.h"
using namespace std;

class RoomSocketHandler : public TCPSocketHandler
{
private:
    map<int,shared_ptr<Room> >& _rooms;

public:
    RoomSocketHandler(map<int,shared_ptr<Room> >& rooms)
        : _rooms(rooms)
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
        cout << "Received " << what << " from [" << fd << "]." << endl;
    }

    void handle_send_error(int fd, const char* what, size_t nbytes)
    {
        cout << "Failed to send " << what << " to [" << fd << "]." << endl;
    }

    void handle_select_error()
    {
    }
};

class UserSocketHandler : public TCPSocketHandler
{
private:
    map<int,shared_ptr<User> >& _users;

public:
    UserSocketHandler(map<int,shared_ptr<User> >& users)
        : _users(users)
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
        cout << "Received " << what << " from user [" << fd << "]." << endl;
    }

    void handle_send_error(int fd, const char* what, size_t nbytes)
    {
        cout << "Failed to send " << what << " to user [" << fd << "]." << endl;
    }

    void handle_select_error()
    {
    }
};


class Server
{
private:
    RoomSocketHandler _rooms_handler;
    UserSocketHandler _users_handler;

    TCPSocket _rooms_socket;
    TCPSocket _users_socket;

    map<int,shared_ptr<Room> > _rooms;
    map<int,shared_ptr<User> > _users;

public:
    Server(const char* rport, const char* uport)
        :   _rooms_handler(_rooms),
            _users_handler(_users),
            _rooms_socket(rport, &_rooms_handler),
            _users_socket(uport, &_users_handler)
    {
        for(;;) {
            _users_socket.go();
            _rooms_socket.go();
            usleep(100000);
        }
    }
};

#endif
