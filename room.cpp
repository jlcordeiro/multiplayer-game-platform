#include "room.h"
#include <limits>
#include "messages.h"

void GameRoom::dispatch() {
    while (1) {
        char buffer[BUF_SIZE] = "";

        vector<string> new_messages;
        int res = _socket.recv(new_messages);
        cout << "n: " << res << endl;
        if (res < 0) {
            return;
        }

        for (auto buffer : new_messages) {
            std::string err;
            cout << "<< " << buffer << endl;

            auto json = Json::parse(buffer, err);
            if (protocol::Join::validate_reply(buffer)) {
                string user_name = json[protocol::Join::tag]["user"].string_value();
                cout << user_name << " joined!" << endl;

                shared_ptr<Entity> user = shared_ptr<Entity>(new Entity(USER, 1));
                user->setName(user_name);
                addRelation(user);
            }

            if (protocol::Quit::validate_reply(buffer)) {
                string user_name = json[protocol::Quit::tag]["user"].string_value();
                cout << user_name << " has quit!" << endl;

                auto user = getRelationByName(user_name);
                removeRelation(user);
            }

            if (protocol::RVar::validate(buffer)) {
                handleVariable<Entity>(shared_from_this(), json[protocol::RVar::tag]);
            }

            if (protocol::UVar::validate(buffer)) {
                handleVariable<Entity>(_relations, json[protocol::UVar::tag]);
            }
        }

        for (auto varp : getVariables()) {
            cout << "~ " << varp.first << " => " << varp.second << endl;
        }

        for (auto u : _relations) {
            cout << "<<< [U] " << u.second->getName() << endl;
            for (auto varp : u.second->getVariables()) {
                cout << "      - " << varp.first << " => " << varp.second << endl;
            }
        }

        sleep(1);
    }
}
