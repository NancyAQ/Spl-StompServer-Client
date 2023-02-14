#pragma once
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include "../include/event.h"
#include <string>
class ClientInput{
    private: 
    ConnectionHandler &Handler;
     StompProtocol &protocol;
    public:
    ClientInput( ConnectionHandler& Handler, StompProtocol &protocol);
    void run();
    void loginParameters(std::string input);
    void joinParameters(std::string input);
    void exitParameters(std::string input);
     void logoutParameters(std::string input);
    void reportParameters(std::string input);
    void summaryParameters(std::string input);

};