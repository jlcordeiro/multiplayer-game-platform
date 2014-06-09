#include <unistd.h>
#include "server.h"

int main(int argc, const char *argv[])
{
    if (argc != 2) {
        return 1;
    }

    const char* PORT = argv[1];

    TCPServer server(PORT);

    for(;;) {
        server.go();
        usleep(100000);
    }

    return 0;
}
