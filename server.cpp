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

        for (auto u : _users) {
            cout << "<<< [U] " << u.second->getName() << endl;
            for (auto varp : u.second->getVariables()) {
                cout << "      - " << varp.first << " => " << varp.second << endl;
            }
        }

        for (auto rp : _rooms) {
            shared_ptr<Room> r = rp.second;
            if (r == nullptr) continue;

            cout << "<<< [R] " << r->getName()
                 << " <" << r->getUserCount() << "/" << r->getMaxUsers() << ">" << endl;
            for (auto varp : rp.second->getVariables()) {
                cout << "      - " << varp.first << " => " << varp.second << endl;
            }
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
        if (findByName<Room>(_rooms, name) != nullptr) {
            return handle_room_disconnect(fd);
        }

        room->setName(name);
        return;
    }

    if (protocol::MaxUsers::validate(data)) {
        room->setMaxUsers(json["maxUsers"].int_value());
        return;
    }

    if (protocol::RVar::validate(data)) {
        handleVariable<Room>(_rooms, json[protocol::RVar::tag]);

        for (auto room_entry : _rooms) {
            auto room = room_entry.second;
            send(room->getFd(), data);

            for (auto user_entry : room->getUsers()) {
                send(user_entry.second->getFd(), data);
            }
        }

    }

    if (protocol::UVar::validate(data)) {
        handleVariable<User>(_users, json[protocol::UVar::tag]);

        for (auto room_entry : _rooms) {
            auto room = room_entry.second;
            send(room->getFd(), data);

            for (auto user_entry : room->getUsers()) {
                send(user_entry.second->getFd(), data);
            }
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

    if (protocol::Join::validate_request(data)) {
        auto room = findByName<Room>(_rooms, json[protocol::Join::tag].string_value());
        if (room && !room->containsUser(user)
                 && room->getUserCount() < room->getMaxUsers()) {

            // announce the join
            auto announce_join = [&] (int fd) {
                send(fd, protocol::Join::reply(room->getName(), user->getName()));
            };

            announce_join(room->getFd());
            for (auto userpair : room->getUsers()) {
                announce_join(userpair.second->getFd());
                // tell the new user who was already on the room
                send(fd, protocol::Join::reply(room->getName(), userpair.second->getName()));
            }
            // send the room variables to the new user
            for (auto varp : room->getVariables()) {
                send(fd, protocol::RVar::str(room->getName(), varp.first, varp.second));
            }

            // add the user to the room
            room->addUser(user);
        }
        return;
    }

    if (protocol::Quit::validate_request(data)) {
        auto room = findByName<Room>(_rooms, json[protocol::Quit::tag].string_value());
        if (room && room->containsUser(user)) {
            room->removeUser(user);
        }
        return;
    }

    if (protocol::UVar::validate(data)) {
        handleVariable<User>(_users, json[protocol::UVar::tag]);

        for (auto room_entry : _rooms) {
            auto room = room_entry.second;
            send(room->getFd(), data);

            for (auto user_entry : room->getUsers()) {
                send(user_entry.second->getFd(), data);
            }
        }
    }
}

void Server::handle_room_connect(int fd)
{
    cout << "Adding room [" << fd << "]." << endl;
    shared_ptr<Room> room = shared_ptr<Room>(new Room());
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
    shared_ptr<User> user = shared_ptr<User>(new User());
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
        send(fd, protocol::Quit::reply(room_name, user->getName()));
    };

    for (auto r : _rooms) {
        int fd = r.first;
        auto room = r.second;
        const string& room_name = room->getName();

        announce_quit(fd, room_name);
        for (auto userpair : room->getUsers()) {
            announce_quit(userpair.second->getFd(), room_name);
        }

        room->removeUser(user);
    }

    // delete user
    _users.erase(fd);
}
