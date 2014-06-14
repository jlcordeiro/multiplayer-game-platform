#include <unistd.h>
#include "server.h"

unsigned long Entity::_id_current = 0;

int main(int argc, const char *argv[])
{
    if (argc != 3) {
        return 1;
    }

    const char* RPORT = argv[1];
    const char* UPORT = argv[2];

    Server server(RPORT, UPORT);

    return 0;
}
