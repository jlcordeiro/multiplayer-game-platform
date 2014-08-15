#include "room.h"
#include <limits>
#include "messages.h"

// void GameRoom::dispatch(Entity e, const string& buffer) {
//     std::string err;
// 
//     auto json = Json::parse(buffer, err);
//     if (protocol::Join::validate_reply(buffer)) {
//         string user_name = json[protocol::Join::tag]["user"].string_value();
//         cout << user_name << " joined!" << endl;
// 
//         shared_ptr<Entity> user = shared_ptr<Entity>(new Entity(USER, 1));
//         user->setName(user_name);
//         e->addRelation(user);
//     }
// 
//     if (protocol::Quit::validate_reply(buffer)) {
//         string user_name = json[protocol::Quit::tag]["user"].string_value();
//         cout << user_name << " has quit!" << endl;
// 
//         auto user = getRelationByName(user_name);
//         e->removeRelation(user);
//     }
// 
//     if (protocol::RVar::validate(buffer)) {
//         e->handleVariable<Entity>(shared_from_this(), json[protocol::RVar::tag]);
//     }
// 
//     if (protocol::UVar::validate(buffer)) {
//         e->handleVariable<Entity>(_relations, json[protocol::UVar::tag]);
//     }
// }
