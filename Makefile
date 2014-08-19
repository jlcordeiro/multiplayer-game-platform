GCC := gcc
GPP := g++ -std=c++11 -O0 -ggdb

IPATH := -I./deps/

.PHONY: deps

all: deps main game client

deps:
	make -C ./deps/

psocket: psocket.c psocket.h
	$(GCC) psocket.c -c

tcpsocket: psocket tcpsocket.cpp tcpsocket.h messages
	$(GPP) tcpsocket.cpp -c $(IPATH)

entity: entity.cpp entity.h
	$(GPP) entity.cpp -c $(IPATH)

messages: messages.cpp messages.h 
	$(GPP) messages.cpp -c $(IPATH)

server: server.h entity messages
	$(GPP) server.cpp -c $(IPATH)

main: tcpsocket server
	$(GPP) main.cpp *.o deps/json11/json11.o $(IPATH) $(LDFLAGS) -o main

game: tcpsocket server
	$(GPP) gameserver.cpp *.o deps/json11/json11.o $(IPATH) $(LDFLAGS) -o game

client: tcpsocket server
	$(GPP) gameclient.cpp *.o deps/json11/json11.o $(IPATH) $(LDFLAGS) -o client

clean:
	rm *.o main
