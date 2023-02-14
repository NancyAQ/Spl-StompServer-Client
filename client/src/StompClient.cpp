#include <thread>
#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/ClientInput.h"
#include "../include/SocketInput.h"
#include "../include/StompProtocol.h"
bool shouldClose=false;
bool logout=false;
volatile bool notlogged=true;
bool FirstUser=false;
int main(int argc, char *argv[]) {
    while(true){
        shouldClose=false;
        logout=false;
        notlogged=true;
        FirstUser=false;
        printf("first loop\n");
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
            return -1;
        }
        std::string host = argv[1];
        short port = atoi(argv[2]);
        while(!shouldClose){
            if(notlogged){
                std::cout << "FirstUser is in login: " << FirstUser << std::endl;
                ConnectionHandler handler(host, port);
                StompProtocol protocol(handler);
                if (!handler.connect()) {
                    std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
                    return 1;
                }
                ClientInput keyInput(handler,protocol);
                SocketInput sockInput(handler);
                std::thread keyThread (&ClientInput::run,&keyInput);
                std::thread sockThread(&SocketInput::run,&sockInput);
                keyThread.join();
                sockThread.join();
            }
        }
    }
	return 0;
}