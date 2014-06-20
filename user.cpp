#include "user.h"

void GameUser::dispatch()
{
    setName("Jack");
    joinRoom("Room1");

    while (1) {
        char buffer[BUF_SIZE] = "";

        size_t nbytes = _socket.recv(buffer);
        if (nbytes > 0) {
            cout << "<< " << buffer << endl;
        }

        sleep(1);
    }
}
