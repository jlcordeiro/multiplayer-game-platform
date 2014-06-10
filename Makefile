GCC := gcc
GPP := g++

all: main

psocket: psocket.c psocket.h
	$(GCC) psocket.c -c

server: psocket server.cpp server.h
	$(GPP) server.cpp -c

main: server
	$(GPP) main.cpp server.o psocket.o -o main

clean:
	rm server.o main
