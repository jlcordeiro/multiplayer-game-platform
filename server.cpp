#include "server.h"
#include "messages.h"

Server::Server(const char* rport, const char* uport)
:   _rooms_socket(rport, _rooms),
    _users_socket(uport, _users)
{
    function<void(shared_ptr<User> user, const string& data)> f =
        std::bind( &Server::process_user_data, this, std::placeholders::_1, std::placeholders::_2);
    _users_socket.set_process_data_fn(f);
    

    for(;;) {
        _users_socket.go();
        _rooms_socket.go();
        usleep(100000);
    }
}

void Server::process_user_data(shared_ptr<User> user, const string& data)
{
    cout << "Received data on the server side! " << data  << endl;
}
