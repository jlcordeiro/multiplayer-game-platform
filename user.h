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

    void setName(string value) override
    {
        User::setName(value);
        _socket.send(protocol::Name::str(value));
    }
private:
    TCPClient _socket;
};

#endif
