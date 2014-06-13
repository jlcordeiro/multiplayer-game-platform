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
    RoomSocket _rooms_socket;
    UserSocket _users_socket;

    map<int,shared_ptr<Room> > _rooms;
    map<int,shared_ptr<User> > _users;

    void process_user_data(shared_ptr<User> user, const string& data);
    void process_room_data(shared_ptr<Room> user, const string& data);

public:
    Server(const char* rport, const char* uport);
};

#endif
