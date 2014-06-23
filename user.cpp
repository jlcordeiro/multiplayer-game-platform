#include "user.h"
#include "room.h"

void GameUser::dispatch()
{
    srand(time(NULL));
    string user_name;
    for (int i = 0; i < 5; ++i)
        user_name += char(65 + rand() % 26 + 32 * (rand() % 2));

    setName(user_name);
    joinRoom("Room1");
    setVariable("credit", "1050");

    while (1) {
        vector<string> new_messages;
        int res = _socket.recv(new_messages);
//         if (res < 0) {
//             return;
//         }

        for (auto buffer : new_messages) {
            cout << "<< " << buffer << endl;

            string err;
            auto json = Json::parse(buffer, err);
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

            if (protocol::UVar::validate(buffer)) {
                auto obj = json[protocol::UVar::tag];
                string user_name = obj["user"].string_value();
                string name = obj["name"].string_value();
                string value = obj["value"].string_value();

                auto user = _my_room->getUserByName(user_name);
                user->setVariable(name, value);
            }
        }

        cout << " - " << _my_room->getName() << " - " << endl;
        for (auto varp : _my_room->getVariables()) {
            cout << "      ~ " << varp.first << " => " << varp.second << endl;
        }
        for (auto u : _my_room->getUsers()) {
            auto user = u.second;
            cout << "<<< [:] " << user->getName() << endl;
            for (auto varp : user->getVariables()) {
                cout << "      - " << varp.first << " => " << varp.second << endl;
            }
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

void GameUser::setVariable(const string& name, const string& value)
{
    User::setVariable(name, value);
    _socket.send(protocol::UVar::str(getName(), name, value));
}

