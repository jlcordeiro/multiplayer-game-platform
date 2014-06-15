#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <iostream>
#include <unistd.h>
#include <functional>
#include "room.h"
#include "user.h"
#include "tcpsocket.h"

template<class T>
shared_ptr<T> findByName(map<int,shared_ptr<T> > group, string name)
{
    for (auto entity : group) {
        if (entity.second->getName() == name) {
            return entity.second;
        }
    }

    return nullptr;
}

class Server
{
private:
    TCPServer _rooms_socket;
    TCPServer _users_socket;

    map<int,shared_ptr<Room> > _rooms;
    map<int,shared_ptr<User> > _users;

    void handle_user_data(int fd, const string& data);
    void handle_room_data(int fd, const string& data);
    void handle_user_connect(int fd);
    void handle_room_connect(int fd);
    void handle_user_disconnect(int fd);
    void handle_room_disconnect(int fd);

public:
    Server(const char* rport, const char* uport);
};

#endif
