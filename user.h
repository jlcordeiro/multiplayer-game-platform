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
        setName("Jack");
        joinRoom("Room1");

        while (1) {
            char buffer[BUF_SIZE] = "";

            size_t nbytes = _socket.recv(buffer);
            if (nbytes > 0) {
                cout << "<< " << buffer << endl;
            }

            sleep(1);
        }
    }

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
