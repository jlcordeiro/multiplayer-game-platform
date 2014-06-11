#include <unistd.h>
#include "tcpsocket.h"

int main(int argc, const char *argv[])
{
    if (argc != 2) {
        return 1;
    }

    const char* PORT = argv[1];

    TCPSocket server(PORT);

    for(;;) {
        server.go();
        usleep(100000);
    }

    return 0;
}
