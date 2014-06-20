#ifndef __USER_H__
#define __USER_H__

#include "messages.h"
#include "config.h"
#include "entity.h"

class User : public Entity
{
public:
    User()
        : Entity()
    {
    }
};

class GameUser : public User
{
public:
    GameUser()
        : User(),
          _socket("localhost", atoi(Config::USER_PORT))
    {
    }

    void dispatch();

    void setName(string value) override
    {
        User::setName(value);
        _socket.send(protocol::Name::str(value));
    }

    void joinRoom(string name)
    {
        _room_name = name;
        _socket.send(protocol::Join::request(name));
    }

private:
    TCPClient _socket;
    string _room_name;
};

#endif
