#include "user.h"
#include "room.h"

void GameUser::dispatch()
{
    setName("Jack");
    joinRoom("Room1");

    while (1) {
        char buffer[BUF_SIZE] = "";

        size_t nbytes = _socket.recv(buffer);
        if (nbytes > 0) {
            cout << "<< " << buffer << endl;

            string err;
            auto json = Json::parse(string(buffer), err);
            if (protocol::Join::validate_reply(buffer)) {
                string user_name = json[protocol::Join::tag]["user"].string_value();
                cout << user_name << " joined!" << endl;

                shared_ptr<User> user = shared_ptr<User>(new User());
                user->setName(user_name);
                _my_room->addUser(user);
            }

            if (protocol::Quit::validate_reply(buffer)) {
                string user_name = json[protocol::Quit::tag]["user"].string_value();
                cout << user_name << " has quit!" << endl;

                auto user = _my_room->getUserByName(user_name);
                _my_room->removeUser(user);
            }
        }

        cout << " - " << _my_room->getName() << " - " << endl;
        for (auto varp : _my_room->getVariables()) {
            cout << "      - " << varp.first << " => " << varp.second << endl;
        }
        for (auto u : _my_room->getUsers()) {
            auto user = u.second;
            cout << "<<< [:] " << user->getName() << endl;
        }

        sleep(1);
    }
}

void GameUser::joinRoom(const string& name)
{
    _my_room = shared_ptr<Room>(new Room());
    _my_room->setName(name);
    _my_room->addUser(shared_ptr<User>(this));
    _socket.send(protocol::Join::request(name));
}
