#include "config.h"
#include "entity.h"
#include "tcpsocket.h"
#include <string>

int main(int argc, const char *argv[])
{
    if (argc < 3) {
        return 1;
    }

    auto socket = shared_ptr<TCPClient>(new TCPClient("localhost", atoi(Config::ROOM_PORT), false));
    auto g = shared_ptr<SEntity>(new SEntity(socket, 10));

    g->acceptMessageVisitor(&printMessage);
    g->acceptMessageVisitor(&handleJoin);

    g->setName(string(argv[1]));
    g->setMaxRelations(4);
    g->setVariable("var1", string(argv[2]));

    g->dispatch();
    return 0;
}
