#include "../include/SocketInput.h"
#include  <sstream>
extern bool shouldClose;
extern bool logout;
extern bool FirstUser;
extern volatile bool notlogged;
SocketInput::SocketInput(ConnectionHandler &Handler):Handler(Handler){}

void SocketInput::run(){
    while(!shouldClose){
        std::string response;
        if(Handler.getFrameAscii(response,'\0')){
            std::string firstWord;
            std::stringstream ss(response);
            ss >> firstWord;
            if(firstWord == "CONNECTED"){
             std::cout << response << std::endl;
             FirstUser=true;

             }

            else if(firstWord == "RECEIPT"){
            std::cout << response << std::endl;
            }

            else if(firstWord == "MESSAGE"){
            std::cout << response << std::endl;
                  
            }

            else if(firstWord == "ERROR"){
                std::stringstream ss(response);
                std::string line;
                while (std::getline(ss, line)){
                    if (line.find("message:") == 0){   
                        std::cout << "Error message received is:" << std::endl;
                        std::string message = line.substr(8);
                        std::cout << "message: " << message << std::endl;
                        break;
                    }
                }
                shouldClose = true;
            }
        }
    }
}
