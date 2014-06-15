#include "room.h"

int main(int argc, const char *argv[])
{

    GameRoom g;
    g.setName("Test Game - Room 1");
    sleep(3);
    g.setMaxUsers(4);

    return 0;
}
