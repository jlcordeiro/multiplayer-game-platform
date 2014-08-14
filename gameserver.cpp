#include "room.h"
#include <string>

int main(int argc, const char *argv[])
{
    if (argc < 3) {
        return 1;
    }

    auto g = shared_ptr<GameRoom>(new GameRoom());

    g->setName(string(argv[1]));
    g->setMaxRelations(4);
    g->setVariable("var1", string(argv[2]));

    g->dispatch();
    return 0;
}
