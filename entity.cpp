#include "entity.h"

unsigned long Entity::_id_current = 0;

void Entity::print()
{
    cout << " ------ " << endl;
    cout << "[" << _fd << "/" << getName() << "]" << endl;
    for (auto varp : getVariables()) {
        cout << "~ " << varp.first << " => " << varp.second << endl;
    }

    for (auto u : _relations) {
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
        int res = _communication->recv(new_messages);
        if (res < 0) {
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
        auto comm = shared_ptr<Communication>(new NoCommunication());
        auto user = shared_ptr<Entity>(new Entity(comm, 1));
        user->setName(user_name);
        room.addRelation(user);
    }
}

void handleQuit(Entity& room, const std::string& buffer)
{
    if (protocol::Quit::validate(buffer)) {
        string err;
        auto json = Json::parse(buffer, err);
        string user_name = json[protocol::Quit::tag]["user"].string_value();
        auto user = room.getRelationByName(user_name);
        room.removeRelation(user);
    }
}

shared_ptr<Entity> getRoom(Entity& e)
{
    if (e.getRelationCount() > 0) {
        return e.getRelations().begin()->second;
    }

    return nullptr;
}
