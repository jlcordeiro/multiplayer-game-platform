#include "user.h"

// void GameUser::dispatch(Entity e, const string& buffer)
// {
//     string err;
//     auto json = Json::parse(buffer, err);
//     if (protocol::RVar::validate(buffer)) {
//         e->handleVariable<Entity>(_my_room, json[protocol::RVar::tag]);
//     }
// 
//     if (protocol::UVar::validate(buffer)) {
//         e->handleVariable<Entity>(_my_room->getUsers(), json[protocol::UVar::tag]);
//     }
// }
