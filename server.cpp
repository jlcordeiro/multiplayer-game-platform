#include "server.h"
#include "messages.h"
using namespace std;
using namespace std::placeholders;

Server::Server(const char* rport, const char* uport)
:   _rooms_socket(rport),
    _users_socket(uport)
{
    auto h_users_data = sp_data_fn_t(
            new data_fn_t(bind(&Server::handle_user_data, this, _1, _2)));
    auto h_rooms_data = sp_data_fn_t(
            new data_fn_t(bind(&Server::handle_room_data, this, _1, _2)));

    auto h_users_connect = sp_conn_fn_t(
            new conn_fn_t(bind(&Server::handle_user_connect, this, _1)));
    auto h_rooms_connect = sp_conn_fn_t(
            new conn_fn_t(bind(&Server::handle_room_connect, this, _1)));

    auto h_users_disconnect = sp_conn_fn_t(
            new conn_fn_t(bind(&Server::handle_user_disconnect, this, _1)));
    auto h_rooms_disconnect = sp_conn_fn_t(
            new conn_fn_t(bind(&Server::handle_room_disconnect, this, _1)));

    _users_socket.set_data_fn(h_users_data);
    _rooms_socket.set_data_fn(h_rooms_data);
    _users_socket.set_connect_fn(h_users_connect);
    _rooms_socket.set_connect_fn(h_rooms_connect);
    _users_socket.set_disconnect_fn(h_users_disconnect);
    _rooms_socket.set_disconnect_fn(h_rooms_disconnect);

    for(;;) {
        _users_socket.go();
        _rooms_socket.go();

//         for (auto up : _users) {
//             up.second->print();
//         }

        for (auto rp : _rooms) {
            rp.second->print();
        }
        cout << endl << endl;

        usleep(1000000);
    }
}

void Server::handle_room_data(int fd, const string& data)
{
    auto room = _rooms[fd];

    if (!room) {
        return;
    }

    cout << "[" << fd << "/" << room->getName() << "] >> " << data << endl;

    std::string err;
    auto json = Json::parse(data, err);

    if (protocol::Name::validate(data)) {
        auto name = json[protocol::Name::tag].string_value();
        if (findByName<Entity>(_rooms, name) != nullptr) {
            cout << "Error in setName" << endl;
            return handle_room_disconnect(fd);
        }

        room->setName(name);
        return;
    }

    if (protocol::MaxUsers::validate(data)) {
        room->setMaxRelatives(json["maxUsers"].int_value());
        return;
    }

    if (protocol::Var::validate(data)) {
        string to_name = json[protocol::Var::tag][protocol::Var::to_tag].string_value();
        if (room->getName() == to_name) {
            handleVariable(room, json[protocol::Var::tag]);
            room->broadcast(data);
        }
    }
}

void Server::handle_user_data(int fd, const string& data)
{
    auto user = _users[fd];

    cout << "[" << fd << "/" << user->getName() << "] >> " << data << endl;

    std::string err;
    auto json = Json::parse(data, err);

    if (protocol::Name::validate(data)) {
        user->setName(json[protocol::Name::tag].string_value());
        return;
    }

    if (protocol::Join::validate(data)) {
        auto room = findByName<Entity>(_rooms, json[protocol::Join::tag]["room"].string_value());
        if (!room) {
            return;
        }

        auto send_variables = [&](Entity& e)
        {
            for (auto varp : e.getVariables()) {
                send(fd, protocol::Var::str("", e.getName(), varp.first, varp.second));
            }
        };

        Relatives<Entity>& relatives = room->relatives();
        if (relatives.add(user)) {
            room->broadcast(data);
            // tell the new user who was already on the room and its variables
            for (auto userpair : relatives.get()) {
                auto ruser = userpair.second;
                if (ruser->getName() != user->getName()) {
                    send(fd, protocol::Join::str(room->getName(), ruser->getName()));
                }
                send_variables(*ruser);
            }
            send_variables(*room);
        }
        return;
    }

    if (protocol::Quit::validate(data)) {
        auto room = findByName<Entity>(_rooms, json[protocol::Quit::tag]["room"].string_value());
        if (!room) {
            return;
        }

        Relatives<Entity>& relatives = room->relatives();
        if (relatives.remove(user)) {
            room->broadcast(data);
            // tell the new user who was already on the room
            for (auto userpair : relatives.get()) {
                send(fd, protocol::Quit::str(room->getName(), userpair.second->getName()));
            }
        }

        return;
    }

    if (protocol::Var::validate(data)) {
        string to_name = json[protocol::Var::tag][protocol::Var::to_tag].string_value();
        if (user->getName() == to_name) {
            handleVariable(user, json[protocol::Var::tag]);
            for (auto room : _rooms) { // send only to the rooms containing the user.
                room.second->broadcast(data);
            }
        }
    }
}

void Server::handle_room_connect(int fd)
{
    cout << "Adding room [" << fd << "]." << endl;
    auto room = shared_ptr<Entity>(new Entity(1000));
    room->setFd(fd);
    _rooms[fd] = room;
}

void Server::handle_room_disconnect(int fd)
{
    cout << "Removing room [" << fd << "]." << endl;
    _rooms.erase(fd);
}

void Server::handle_user_connect(int fd)
{
    cout << "Adding user [" << fd << "]." << endl;
    auto user = shared_ptr<Entity>(new Entity(1));
    user->setFd(fd);
    _users[fd] = user;
}

void Server::handle_user_disconnect(int fd)
{
    cout << "Removing user [" << fd << "]." << endl;

    // remove from rooms
    auto user = _users[fd];

    // announce player is leaving
    auto announce_quit = [&] (int fd, const string& room_name) {
        send(fd, protocol::Quit::str(room_name, user->getName()));
    };

    for (auto r : _rooms) {
        int fd = r.first;
        auto room = r.second;
        const string& room_name = room->getName();

        announce_quit(fd, room_name);
        for (auto userpair : room->relatives().get()) {
            announce_quit(userpair.second->getFd(), room_name);
        }

        room->relatives().remove(user);
    }

    // delete user
    _users.erase(fd);
}
