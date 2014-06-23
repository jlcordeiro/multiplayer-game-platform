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
        shared_ptr<User> getUserByName(const string& name);
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

    void setMaxUsers(long int value) override
    {
        Room::setMaxUsers(value);
        _socket.send(protocol::MaxUsers::str(value));
    }

    void setName(string value) override
    {
        Room::setName(value);
        _socket.send(protocol::Name::str(value));
    }

    void setVariable(const string& name, const string& value) override
    {
        Room::setVariable(name, value);
        _socket.send(protocol::RVar::str(getName(), name, value));
    }
private:
    TCPClient _socket;
};

#endif
