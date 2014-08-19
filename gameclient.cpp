#include "config.h"
#include "entity.h"
#include "tcpsocket.h"

int main(int argc, const char *argv[])
{
    auto socket = shared_ptr<TCPClient>(new TCPClient("localhost", atoi(Config::USER_PORT), false));
    auto u = shared_ptr<SEntity>(new SEntity(socket, 1));

    srand(time(NULL));
    string user_name;
    for (int i = 0; i < 5; ++i) {
        user_name += char(65 + rand() % 26 + 32 * (rand() % 2));
    }

    u->setName(user_name);
    u->joinRoom("Room1");
    u->setVariable("credit", "1050");

    u->dispatch();
    return 0;
}
