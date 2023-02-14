#pragma once
#include "ConnectionHandler.h"
class SocketInput{
    private:
    ConnectionHandler &Handler;
    public:
    SocketInput(ConnectionHandler &Handler);
    void run();
   
};