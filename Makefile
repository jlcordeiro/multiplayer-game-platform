GCC := gcc
GPP := g++

all: main

psocket: psocket.c psocket.h
	$(GCC) psocket.c -c

tcpsocket: psocket tcpsocket.cpp tcpsocket.h
	$(GPP) tcpsocket.cpp -c

user: user.cpp user.h
	$(GPP) user.cpp -c

room: user room.cpp room.h
	$(GPP) room.cpp -c

main: tcpsocket room user
	$(GPP) main.cpp tcpsocket.o psocket.o room.o user.o -o main

clean:
	rm tcpsocket.o main
