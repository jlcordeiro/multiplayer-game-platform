#include "config.h"
#include "entity.h"
#include "tcpsocket.h"
#include <string>
#include <thread>

int main(int argc, const char *argv[])
{
    if (argc < 3) {
        return 1;
    }

    TCPClient socket("localhost", atoi(Config::ROOM_PORT), false);
    auto comm = shared_ptr<Communication>(new TCPCommunication(socket));
    auto g = shared_ptr<Entity>(new Entity(comm, 10));

    auto print_buffer = [] (Entity& sender, const std::string& buffer)
                        { cout << "[" << sender.getName() << "] >> " << buffer << endl; };

    auto handle_var = [] (Entity& sender, const std::string& buffer)
    {
        if (protocol::Var::validate(buffer)) {
            std::string err;
            auto json = Json::parse(buffer, err);
            handleVariable(sender.relatives().get(), json[protocol::Var::tag]);
        }
    };

    g->acceptMessageVisitor(print_buffer);
    g->acceptMessageVisitor(handleJoin);
    g->acceptMessageVisitor(handleQuit);
    g->acceptMessageVisitor(handle_var);

    g->setName(string(argv[1]));
    g->setMaxRelatives(4);
    g->setVariable("var1", string(argv[2]));

    thread dispatch([&](){g->dispatch();});

    while (true) {
        g->print();
        usleep(500000);
    }

    return 0;
}
