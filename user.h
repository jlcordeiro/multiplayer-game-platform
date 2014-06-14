#ifndef __USER_H__
#define __USER_H__

#include "entity.h"

class User : public Entity
{
public:
    User(long int fd)
        : Entity(fd)
    {
    }
};

typedef function<void(shared_ptr<User>, const string&)> user_data_fn_t;

class UserSocket : public TCPSocket
{
private:
    map<int,shared_ptr<User> >& _users;
    weak_ptr<user_data_fn_t> _server_data_fn;

public:
    UserSocket(const char* port, map<int,shared_ptr<User> >& users)
        : TCPSocket(port),
          _users(users)
    {
    }

    void handle_connect(int fd)
    {
        cout << "Adding user [" << fd << "]." << endl;
        _users[fd] = shared_ptr<User>(new User(fd));
    }

    void handle_disconnect(int fd)
    {
        cout << "Removing user [" << fd << "]." << endl;
        _users.erase(fd);
    }

    void handle_data(int fd, const char* what, size_t nbytes)
    {
        if (auto pfn = _server_data_fn.lock()) {
            (*pfn)(_users[fd], string(what));
        }
    }

    void handle_select_error()
    {
    }

    void set_process_data_fn(shared_ptr<user_data_fn_t> f)
    {
        _server_data_fn = f;
    }
};

#endif
