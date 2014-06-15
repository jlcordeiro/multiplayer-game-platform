#include <unistd.h>
#include "server.h"
#include "config.h"

int main(int argc, const char *argv[])
{
    Server server(Config::ROOM_PORT, Config::USER_PORT);
    return 0;
}
