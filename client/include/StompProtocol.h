#pragma once
#include "../include/ConnectionHandler.h"
#include "../include/event.h"
#include <atomic>
#include <unordered_map>
#include <vector>
class StompProtocol {
private:
    ConnectionHandler& Handler;
    std::atomic_int joinId;
    std::atomic_int receiptId;
    std::unordered_map<std::string, std::string> gameSubscriptions;//maps game name to its subscription id
    
public: 
    StompProtocol( ConnectionHandler& Handler);
    void HandleLogin(std::string hostPort,std::string username, std::string password);
    void HandleJoin(std::string gameName);
    void HandleExit(std::string gameName);
    void HandleLogout();
    void HandleReport(std::string file);
    void HandleSummary(std::string game_name, std::string user, std::string file);
    std::string covertEventToString(Event evnt);
    std::string dealingWithSummary1(std::string team_a_name,std::string team_b_name , std::vector<Event> gameUpdateForsummary);
    std::string dealingWithSummary2(std::string team_a_name,std::string team_b_name , std::vector<Event> gameUpdateForsummary);
    void addToMap(std::string user,std::vector<Event> gameUpdate);
    std::vector<Event> getfromMap(std::string user);
};
