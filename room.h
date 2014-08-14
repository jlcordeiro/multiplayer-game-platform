#ifndef __ROOM_H__
#define __ROOM_H__

#include "user.h"
#include "config.h"
#include "entity.h"
#include "messages.h"
#include <memory>
#include <limits>
using namespace std;

class GameRoom : public Entity, public std::enable_shared_from_this<GameRoom>
{
public:
    GameRoom()
        : Entity(ROOM, std::numeric_limits<long int>::max()),
          _socket("localhost", atoi(Config::ROOM_PORT))
    {
    }

    void dispatch();


    void setMaxRelations(long int value) override
    {
        Entity::setMaxRelations(value);
        _socket.send(protocol::MaxUsers::str(value));
    }

    void setName(string value) override
    {
        Entity::setName(value);
        _socket.send(protocol::Name::str(value));
    }

    void setVariable(const string& name, const string& value)
    {
        _setVariable(name, value);
        _socket.send(protocol::RVar::str(getName(), name, value));
    }
private:
    TCPClient _socket;
};

#endif
