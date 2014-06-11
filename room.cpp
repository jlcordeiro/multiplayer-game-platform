#include "room.h"

Room::Room(long int id, string name)
    : _id(id),
    _name(name),
    _max_users(),
    _user_count(std::numeric_limits<long int>::max())
{
}

long int Room::getId() const
{
    return _id;
}

string Room::getName() const
{
    return _name;
}

long int Room::getUserCount() const
{
    return _user_count;
}

long int Room::getMaxUsers() const
{
    return _max_users;
}

void Room::setMaxUsers(long int value)
{
    _max_users = value;
}

const map<string, string>& Room::getVariables() const
{
    return _variables;
}

bool Room::containsVariable(string name) const
{
    return (_variables.find(name) != _variables.end());
}

void Room::addUser(shared_ptr<User> user)
{
    _users[user->getId()] = user;
}

bool Room::containsUser(shared_ptr<User> user) const
{
    return (_users.find(user->getId()) != _users.end());
}

void Room::removeUser(shared_ptr<User> user)
{
    _users.erase(user->getId());
}

shared_ptr<User> Room::getUserById(long int id)
{
    map<long int,shared_ptr<User> >::iterator it = _users.find(id);

    if (it == _users.end()) {
        return shared_ptr<User>(NULL);
    }

    return it->second;
}

void Room::setVariable(string name, string value)
{
    _variables[name] = value;
}

void Room::removeVariable(string name)
{
    _variables.erase(name);
}

string Room::getVariable(string name) const
{
    std::map<string,string>::const_iterator it = _variables.find(name);

    if (it == _variables.end()) {
        return NULL;
    }

    return it->second;
}

