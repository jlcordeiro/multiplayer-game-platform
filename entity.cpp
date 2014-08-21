#include "entity.h"

unsigned long Entity::_id_current = 0;

void Entity::print()
{
    cout << " ------ " << endl;
    cout << "[" << _fd << "/" << getName() << "]" << endl;
    for (auto varp : getVariables()) {
        cout << "~ " << varp.first << " => " << varp.second << endl;
    }

    for (auto u : _relatives.get()) {
        cout << "<<< [R] " << u.second->getName() << endl;
        for (auto varp : u.second->getVariables()) {
            cout << "      - " << varp.first << " => " << varp.second << endl;
        }
    }
}

void Entity::dispatch()
{
    while (1) {
        vector<string> new_messages;
        if (_communication->recv(new_messages) < 0) {
            return;
        }

        for (auto visitor : _message_visitors) {
            for (auto buffer : new_messages) {
                visitor(*this, buffer);
            }
        }

        usleep(100000);
    }
}

void handleJoin(Entity& room, const std::string& buffer)
{
    if (protocol::Join::validate(buffer)) {
        string err;
        auto json = Json::parse(buffer, err);
        string user_name = json[protocol::Join::tag]["user"].string_value();
        auto user = shared_ptr<Entity>(new Entity(1));
        user->setName(user_name);
        room.relatives().add(user);
    }
}

void handleQuit(Entity& room, const std::string& buffer)
{
    if (protocol::Quit::validate(buffer)) {
        string err;
        auto json = Json::parse(buffer, err);
        string user_name = json[protocol::Quit::tag]["user"].string_value();
        auto user = findByName<Entity>(room.relatives().get(), user_name);
        room.relatives().remove(user);
    }
}

shared_ptr<Entity> getRoom(Entity& e)
{
    return e.relatives().get().begin()->second;
}
