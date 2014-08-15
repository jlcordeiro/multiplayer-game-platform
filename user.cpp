#include "user.h"

// void GameUser::dispatch(Entity e, const string& buffer)
// {
//     string err;
//     auto json = Json::parse(buffer, err);
//     if (protocol::Join::validate_reply(buffer)) {
//         string user_name = json[protocol::Join::tag]["user"].string_value();
//         cout << user_name << " joined!" << endl;
// 
//         shared_ptr<Entity> user = shared_ptr<Entity>(new Entity(USER, 1));
//         user->setName(user_name);
//         _my_room->addRelation(user);
//     }
// 
//     if (protocol::Quit::validate_reply(buffer)) {
//         string user_name = json[protocol::Quit::tag]["user"].string_value();
//         cout << user_name << " has quit!" << endl;
// 
//         auto user = _my_room->getUserByName(user_name);
//         _my_room->removeUser(user);
//     }
// 
//     if (protocol::RVar::validate(buffer)) {
//         e->handleVariable<Entity>(_my_room, json[protocol::RVar::tag]);
//     }
// 
//     if (protocol::UVar::validate(buffer)) {
//         e->handleVariable<Entity>(_my_room->getUsers(), json[protocol::UVar::tag]);
//     }
// }
