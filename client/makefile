CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: StompWCIClient

StompWCIClient: bin/ConnectionHandler.o bin/StompClient.o bin/ClientInput.o bin/SocketInput.o bin/StompProtocol.o bin/event.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/StompClient.o bin/ClientInput.o bin/SocketInput.o bin/StompProtocol.o bin/event.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/ClientInput.o: src/ClientInput.cpp
	g++ $(CFLAGS) -o bin/ClientInput.o src/ClientInput.cpp

bin/SocketInput.o: src/SocketInput.cpp
	g++ $(CFLAGS) -o bin/SocketInput.o src/SocketInput.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

.PHONY: clean
clean:
	rm -f bin/*
	