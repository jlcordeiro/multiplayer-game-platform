#include "room.h"

int main(int argc, const char *argv[])
{
    auto g = shared_ptr<GameRoom>(new GameRoom());
    g->dispatch();
    return 0;
}
