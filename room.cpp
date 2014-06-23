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

shared_ptr<User> Room::getUserByName(const string& name)
{
    for (auto userpair : _users) {
        auto user = userpair.second;
        if (user->getName() == name) {
            return user;
        }
    }

    return nullptr;
}
const map<int, shared_ptr<User> >& Room::getUsers() const
{
    return _users;
}

// ---

void GameRoom::dispatch() {
    setName("Room1");
    setMaxUsers(4);
    setVariable("var1", "3");

    while (1) {
        char buffer[BUF_SIZE] = "";

        vector<string> new_messages;
        int res = _socket.recv(new_messages);
        //             if (res < 0) {
        //                 return;
        //             }

        for (auto buffer : new_messages) {
            std::string err;
            cout << "<< " << buffer << endl;

            auto json = Json::parse(buffer, err);
            if (protocol::Join::validate_reply(buffer)) {
                string user_name = json[protocol::Join::tag]["user"].string_value();
                cout << user_name << " joined!" << endl;

                shared_ptr<User> user = shared_ptr<User>(new User());
                user->setName(user_name);
                addUser(user);
            }

            if (protocol::Quit::validate_reply(buffer)) {
                string user_name = json[protocol::Quit::tag]["user"].string_value();
                cout << user_name << " has quit!" << endl;

                auto user = getUserByName(user_name);
                removeUser(user);
            }

            if (protocol::UVar::validate(buffer)) {
                auto obj = json[protocol::UVar::tag];
                string user_name = obj["user"].string_value();
                string name = obj["name"].string_value();
                string value = obj["value"].string_value();

                auto user = getUserByName(user_name);
                user->setVariable(name, value);
            }
        }

        for (auto varp : getVariables()) {
            cout << "~ " << varp.first << " => " << varp.second << endl;
        }

        for (auto u : _users) {
            cout << "<<< [U] " << u.second->getName() << endl;
            for (auto varp : u.second->getVariables()) {
                cout << "      - " << varp.first << " => " << varp.second << endl;
            }
        }

        cout << "@@ " << getUserCount() << endl;
        sleep(1);
    }
}
