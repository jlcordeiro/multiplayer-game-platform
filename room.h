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

        while (1) {
            char buffer[BUF_SIZE] = "";

            size_t nbytes = _socket.recv(buffer);
            if (nbytes > 0) {
                std::string err;
                cout << "<< " << buffer << endl;
 
                auto json = Json::parse(string(buffer), err);
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
private:
    TCPClient _socket;
};

#endif
