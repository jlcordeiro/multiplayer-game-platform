#include "config.h"
#include "entity.h"
#include "tcpsocket.h"
#include <thread>

int main(int argc, const char *argv[])
{
    string room_name = "Room1";

    srand(time(NULL));
    string user_name;
    for (int i = 0; i < 5; ++i) {
        user_name += char(65 + rand() % 26 + 32 * (rand() % 2));
    }


    TCPClient socket ("localhost", atoi(Config::USER_PORT), false);
    auto comm = shared_ptr<Communication>(new TCPCommunication(socket));
    auto u = shared_ptr<Entity>(new Entity(comm, 1));


    auto print_buffer = [] (Entity& sender, const std::string& buffer)
                        { cout << "[" << sender.getName() << "] >> " << buffer << endl; };

    auto handle_join = [&] (Entity& sender, const std::string& buffer)
                       { handleJoin(*getRoom(sender), buffer); };

    auto handle_quit = [&] (Entity& sender, const std::string& buffer)
                       { handleQuit(*getRoom(sender), buffer); };

    u->acceptMessageVisitor(print_buffer);
    u->acceptMessageVisitor(handle_join);
    u->acceptMessageVisitor(handle_quit);


    u->setName(user_name);
    u->joinRoom(room_name);
    u->setVariable("credit", "1050");

    thread dispatch([&](){u->dispatch();});

    auto my_room = findByName<Entity>(u->relatives().get(), room_name);
    while (true) {
        my_room->print();
        usleep(500000);
    }

    return 0;
}
