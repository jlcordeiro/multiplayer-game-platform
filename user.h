#ifndef __USER_H__
#define __USER_H__

#include "messages.h"
#include "config.h"
#include "entity.h"

class GameUser : public Entity
{
public:
    GameUser()
        : Entity(USER, 1),
          _socket("localhost", atoi(Config::USER_PORT))
    {
    }

    void dispatch();

    void setName(string value) override
    {
        Entity::setName(value);
        _socket.send(protocol::Name::str(value));
    }

    void joinRoom(const string& name)
    {
        _my_room = shared_ptr<Entity>(new Entity(ROOM, 1000));
        _my_room->setName(name);

        addRelation(_my_room);
        _my_room->addRelation(shared_ptr<Entity>(this));

        _socket.send(protocol::Join::request(name));
    }

    void setVariable(const string& name, const string& value)
    {
        _setVariable(name, value);
        _socket.send(protocol::UVar::str(getName(), name, value));
    }

private:
    TCPClient _socket;
    shared_ptr<Entity> _my_room;
};

#endif
