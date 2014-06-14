#ifndef __ROOM_H__
#define __ROOM_H__

#include "user.h"
#include "entity.h"
#include <memory>
using namespace std;

class Room : public Entity
{
    public:
        Room(long int fd);

        long int getUserCount() const;
        long int getMaxUsers() const;
        void setMaxUsers(long int value);

        void addUser(shared_ptr<User> user);
        bool containsUser(shared_ptr<User> user) const;
        void removeUser(shared_ptr<User> user);
        shared_ptr<User> getUserById(long int id);
        const map<long int, shared_ptr<User> >& getUsers() const;

    protected:
        map<long int,shared_ptr<User> > _users;
        long unsigned int _max_users;
        long unsigned int _user_count;
};

#endif
