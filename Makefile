GCC := gcc
GPP := g++ -std=c++11 -O0 -ggdb

IPATH := -I./deps/
LDFLAGS := -pthread

OBJ =  *.o deps/json11/json11.o deps/anet/anet.c

.PHONY: deps

all: main game client

deps:
	mkdir -p deps
	clib install jb55/anet.c
	cd deps/anet/ && gcc -c anet.c
	make -C ./deps/

tcpsocket: tcpsocket.cpp tcpsocket.h messages
	$(GPP) tcpsocket.cpp -c $(IPATH)

entity: entity.cpp entity.h
	$(GPP) entity.cpp -c $(IPATH)

messages: messages.cpp messages.h 
	$(GPP) messages.cpp -c $(IPATH)

server: server.h entity messages
	$(GPP) server.cpp -c $(IPATH)

main: tcpsocket server
	$(GPP) main.cpp $(OBJ) $(IPATH) $(LDFLAGS) -o main

game: tcpsocket server
	$(GPP) gameserver.cpp $(OBJ) $(IPATH) $(LDFLAGS) -o game

client: tcpsocket server
	$(GPP) gameclient.cpp $(OBJ) $(IPATH) $(LDFLAGS) -o client

clean:
	rm *.o main
