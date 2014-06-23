#ifndef __USER_H__
#define __USER_H__

#include "messages.h"
#include "config.h"
#include "entity.h"

class Room;

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

    void joinRoom(const string& name);

    void setVariable(const string& name, const string& value);

private:
    TCPClient _socket;
    shared_ptr<Room> _my_room;
};

#endif
