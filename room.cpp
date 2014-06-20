#include "room.h"
#include <limits>
#include "messages.h"

Room::Room()
    : Entity(),
    _max_users(std::numeric_limits<long int>::max()),
    _user_count(0)
{
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

void Room::addUser(shared_ptr<User> user)
{
    if (containsUser(user)) {
        return;
    }

    _users[user->getId()] = user;
    _user_count++;

    auto str = protocol::Join::reply(user->getName(), getName());
    for (auto u : _users) {
        send(u.first, str);
    }
}

bool Room::containsUser(shared_ptr<User> user) const
{
    return (_users.find(user->getId()) != _users.end());
}

void Room::removeUser(shared_ptr<User> user)
{
    if (!containsUser(user)) {
        return;
    }

    auto str = protocol::Quit::str(*user);
    for (auto u : _users) {
        send(u.first, str);
    }

    _users.erase(user->getId());
    _user_count--;
}

shared_ptr<User> Room::getUserById(long int id)
{
    auto it = _users.find(id);

    if (it == _users.end()) {
        return shared_ptr<User>(NULL);
    }

    return it->second;
}

const map<int, shared_ptr<User> >& Room::getUsers() const
{
    return _users;
}
