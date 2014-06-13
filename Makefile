GCC := gcc
GPP := g++ -std=c++11

IPATH := -I./deps/

.PHONY: deps

all: deps main

deps:
	make -C ./deps/

psocket: psocket.c psocket.h
	$(GCC) psocket.c -c

tcpsocket: psocket tcpsocket.cpp tcpsocket.h messages
	$(GPP) tcpsocket.cpp -c $(IPATH)

user: user.cpp user.h
	$(GPP) user.cpp -c

room: user room.cpp room.h
	$(GPP) room.cpp -c

messages: messages.cpp messages.h 
	$(GPP) messages.cpp -c $(IPATH)

server: server.h messages
	$(GPP) server.cpp -c $(IPATH)

main: tcpsocket room user server
	$(GPP) main.cpp *.o deps/json11/json11.o $(IPATH) $(LDFLAGS) -o main

clean:
	rm tcpsocket.o main
