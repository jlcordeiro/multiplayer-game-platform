#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <iostream>
#include <unistd.h>
#include <functional>
#include "entity.h"
#include "tcpsocket.h"

class Server
{
private:
    TCPServer _rooms_socket;
    TCPServer _users_socket;

    map<int,shared_ptr<Entity> > _rooms;
    map<int,shared_ptr<Entity> > _users;

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
