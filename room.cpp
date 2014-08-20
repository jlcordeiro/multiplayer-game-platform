#include "room.h"
#include <limits>
#include "messages.h"

// void GameRoom::dispatch(Entity e, const string& buffer) {
//     std::string err;
//     auto json = Json::parse(buffer, err);
//     if (protocol::RVar::validate(buffer)) {
//         e->handleVariable<Entity>(shared_from_this(), json[protocol::RVar::tag]);
//     }
// 
//     if (protocol::UVar::validate(buffer)) {
//         e->handleVariable<Entity>(_relations, json[protocol::UVar::tag]);
//     }
// }
