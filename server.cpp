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
        }

        for (auto rp : _rooms) {
            shared_ptr<Room> r = rp.second;
            cout << "<<< [R] " << r->getName()
                 << " <" << r->getUserCount() << "/" << r->getMaxUsers() << ">" << endl;
        }

        usleep(1000000);
    }
}

void Server::handle_room_data(int fd, const string& data)
{
    auto room = _rooms[fd];

    cout << "[" << fd << "/" << room->getName() << "] >> " << data << endl;

    std::string err;
    auto json = Json::parse(data, err);

    if (isSetName(data, err)) {
        room->setName(json["setName"].string_value());
        return;
    }

    if (isMaxUsers(data, err)) {
        room->setMaxUsers(json["maxUsers"].int_value());
        return;
    }
}

void Server::handle_user_data(int fd, const string& data)
{
    auto user = _users[fd];

    cout << "[" << fd << "/" << user->getName() << "] >> " << data << endl;

    std::string err;
    auto json = Json::parse(data, err);

    if (isSetName(data, err)) {
        user->setName(json["setName"].string_value());
        return;
    }

    if (isJoin(data, err)) {
        auto room = findByName<Room>(_rooms, json["join"].string_value());
        if (room && !room->containsUser(user)
                 && room->getUserCount() < room->getMaxUsers()) {
            room->addUser(user);
        }
        return;
    }

    if (isQuit(data, err)) {
        auto room = findByName<Room>(_rooms, json["quit"].string_value());
        if (room && room->containsUser(user)) {
            room->removeUser(user);
        }
        return;
    }
}

void Server::handle_room_connect(int fd)
{
    cout << "Adding room [" << fd << "]." << endl;
    _rooms[fd] = shared_ptr<Room>(new Room(fd));
}

void Server::handle_room_disconnect(int fd)
{
    cout << "Removing room [" << fd << "]." << endl;
    _rooms.erase(fd);
}

void Server::handle_user_connect(int fd)
{
    cout << "Adding user [" << fd << "]." << endl;
    _users[fd] = shared_ptr<User>(new User(fd));
}

void Server::handle_user_disconnect(int fd)
{
    cout << "Removing user [" << fd << "]." << endl;

    // remove from rooms
    auto user = _users[fd];

    for (auto r : _rooms) {
        r.second->removeUser(user);
    }

    // delete user
    _users.erase(fd);
}
