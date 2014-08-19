#include "entity.h"

unsigned long EntityBase::_id_current = 0;

void printMessage(EntityBase& sender, const std::string& buffer)
{
    cout << "[" << sender.getName() << "] >> " << buffer << endl;
}

void handleJoin(EntityBase& sender, const std::string& buffer)
{
    string err;
    auto json = Json::parse(buffer, err);
    if (protocol::Join::validate_reply(buffer)) {
        string user_name = json[protocol::Join::tag]["user"].string_value();
        cout << user_name << " joined!" << endl;

        //shared_ptr<Entity> user = shared_ptr<Entity>(new Entity(USER, 1));
        //user->setName(user_name);
        //_my_room->addRelation(user);
    }
}
