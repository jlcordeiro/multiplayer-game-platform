#ifndef __ROOM_H__
#define __ROOM_H__

#include "user.h"
#include "config.h"
#include "entity.h"
#include "messages.h"
#include <memory>
using namespace std;

class Room : public Entity
{
    public:
        Room();

        long int getUserCount() const;
        long int getMaxUsers() const;
        virtual void setMaxUsers(long int value);

        void addUser(shared_ptr<User> user);
        bool containsUser(shared_ptr<User> user) const;
        void removeUser(shared_ptr<User> user);
        shared_ptr<User> getUserById(long int id);
        const map<int, shared_ptr<User> >& getUsers() const;

    protected:
        map<int,shared_ptr<User> > _users;
        long unsigned int _max_users;
        long unsigned int _user_count;
};

class GameRoom : public Room
{
public:
    GameRoom()
        : Room(),
          _socket("localhost", atoi(Config::ROOM_PORT))
    {
    }

    void setMaxUsers(long int value) override
    {
        Room::setMaxUsers(value);
        _socket.send(protocol::MaxUsers::str(value));
    }
private:
    TCPClient _socket;
};

#endif
