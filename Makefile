GCC := gcc
GPP := g++

all: main

psocket: psocket.c psocket.h
	$(GCC) psocket.c -c

server: psocket server.cpp server.h
	$(GPP) server.cpp -c

user: user.cpp user.h
	$(GPP) user.cpp -c

room: user room.cpp room.h
	$(GPP) room.cpp -c

main: server room user
	$(GPP) main.cpp server.o psocket.o -o main

clean:
	rm server.o main
