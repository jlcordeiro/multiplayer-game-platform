#ifndef __ROOM_H__
#define __ROOM_H__

#include "user.h"
#include <string>
#include <map>
using namespace std;

class Room
{
    public:
        Room(int fd);

        int getFd() const;

        void setName(string value);
        string getName() const;

        const map<string, string>& getVariables() const;

        long int getUserCount() const;
        long int getMaxUsers() const;
        void setMaxUsers(long int value);

        bool containsVariable(string name) const;
        void setVariable(string name, string value);
        void removeVariable(string name);
        string getVariable(string name) const;

        void addUser(shared_ptr<User> user);
        bool containsUser(shared_ptr<User> user) const;
        void removeUser(shared_ptr<User> user);
        shared_ptr<User> getUserById(long int id);

    protected:
        int _fd;
        string _name;
        map<long int,shared_ptr<User> > _users;
        map<string,string> _variables;


        long unsigned int _max_users;
        long unsigned int _user_count;
};

#endif
