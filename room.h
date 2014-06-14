#ifndef __ROOM_H__
#define __ROOM_H__

#include "user.h"
#include "entity.h"
#include <memory>
using namespace std;

class Room : public Entity
{
    public:
        Room(long int fd);

        long int getUserCount() const;
        long int getMaxUsers() const;
        void setMaxUsers(long int value);

        void addUser(shared_ptr<User> user);
        bool containsUser(shared_ptr<User> user) const;
        void removeUser(shared_ptr<User> user);
        shared_ptr<User> getUserById(long int id);
        const map<long int, shared_ptr<User> >& getUsers() const;

    protected:
        map<long int,shared_ptr<User> > _users;
        long unsigned int _max_users;
        long unsigned int _user_count;
};

typedef function<void(shared_ptr<Room>, const string&)> room_data_fn_t;

class RoomSocket : public TCPSocket
{
private:
    map<int,shared_ptr<Room> >& _rooms;
    weak_ptr<room_data_fn_t> _server_data_fn;

public:
    RoomSocket(const char* port, map<int,shared_ptr<Room> >& rooms)
        : TCPSocket(port),
          _rooms(rooms)
    {
    }

    void handle_connect(int fd)
    {
        cout << "Adding room [" << fd << "]." << endl;
        _rooms[fd] = shared_ptr<Room>(new Room(fd));
    }

    void handle_disconnect(int fd)
    {
        cout << "Removing room [" << fd << "]." << endl;
        _rooms.erase(fd);
    }

    void handle_data(int fd, const char* what, size_t nbytes)
    {
        if (auto pfn = _server_data_fn.lock()) {
            (*pfn)(_rooms[fd], string(what));
        }
    }

    void handle_select_error()
    {
    }

    void set_process_data_fn(shared_ptr<room_data_fn_t> f)
    {
        _server_data_fn = f;
    }
};


#endif
