#include "server.h"
#include "messages.h"
using namespace std;

Server::Server(const char* rport, const char* uport)
:   _rooms_socket(rport, _rooms),
    _users_socket(uport, _users)
{
    auto f_users = bind(&Server::process_user_data, this, placeholders::_1, placeholders::_2);
    auto f_rooms = bind(&Server::process_room_data, this, placeholders::_1, placeholders::_2);

    auto sp_f_users = shared_ptr<user_data_fn_t>(new user_data_fn_t(f_users));
    auto sp_f_rooms = shared_ptr<room_data_fn_t>(new room_data_fn_t(f_rooms));

    _users_socket.set_process_data_fn(sp_f_users);
    _rooms_socket.set_process_data_fn(sp_f_rooms);

    for(;;) {
        _users_socket.go();
        _rooms_socket.go();

        for (auto u : _users) {
            cout << "<<< [U] " << u.second->getName() << endl;
        }

        for (auto r : _rooms) {
            cout << "<<< [R] " << r.second->getName() << " <" << r.second->getUserCount() << ">" << endl;
        }

        usleep(1000000);
    }
}

void Server::process_room_data(shared_ptr<Room> room, const string& data)
{
    cout << "<< " << data  << endl;

    std::string err;
    auto json = Json::parse(data, err);

    if (isSetName(data, err)) {
        room->setName(json["setName"].string_value());
    }

}

void Server::process_user_data(shared_ptr<User> user, const string& data)
{
    cout << "<< " << data  << endl;

    std::string err;
    auto json = Json::parse(data, err);

    if (isSetName(data, err)) {
        user->setName(json["setName"].string_value());
        return;
    }

    if (isJoin(data, err)) {
        auto room = findByName<Room>(_rooms, json["join"].string_value());
        if (room && !room->containsUser(user)) {
            room->addUser(user);
        }
    }
}
