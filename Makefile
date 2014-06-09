GCC := g++

all: main

server: server.cpp server.h
	$(GCC) server.cpp -c

main: server
	$(GCC) main.cpp server.o -o main

clean:
	rm server.o main
