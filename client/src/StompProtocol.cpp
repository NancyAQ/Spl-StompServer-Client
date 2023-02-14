#include "../include/StompProtocol.h"
#include <atomic>
#include <fstream>
#include <vector>
extern bool shouldClose;
extern bool logout;
extern bool FirstUser;
extern volatile bool notlogged;
StompProtocol::StompProtocol(ConnectionHandler &Handler):Handler(Handler), joinId(0), receiptId(0),gameSubscriptions(){}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string currentloggedinuser;// updated in login function
std::unordered_map< std::string, std::vector<Event>> userEvents;// maps user name to its reported events.
std::unordered_map< std::string, std::string> GeneralGameUpdates;

void StompProtocol::HandleJoin(std::string gameName) {
    // Generate unique ids for the join and receipt requests
    std::string joinId = std::to_string(this->joinId++);
    std::string receiptId = std::to_string(this->receiptId++);

    // Build the SUBSCRIBE frame
    std::string frame = "SUBSCRIBE\ndestination:/" + gameName + "\nid:" + joinId + "\nreceipt:" + receiptId + "\n\n\0";

    // Send the frame to the server
    if (!Handler.sendFrameAscii(frame, '\0')) {
        std::cout << "Error sending SUBSCRIBE frame" << std::endl;
        return;
    }

    gameSubscriptions[gameName]=joinId;
   
    
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StompProtocol::HandleLogin(std::string hostPort, std::string username, std::string password) {
    // Build the CONNECT frame
   
    if(FirstUser){
        std::cout << "The client is already logged in, log out before trying again." << std::endl;
        return;
    }
    std::string host="stomp.cs.bgu.ac.il";
    std::string frame = "CONNECT\naccept-version:1.2\nhost:"+host+ "\nlogin:" + username + "\npasscode:" + password + "\n\n\0";
    
    // Send the frame to the server
    if (!Handler.sendFrameAscii(frame, '\0')) {
        std::cout << "Could not connect to server" << std::endl;
        return;
    }
    else{
    notlogged=false;
    currentloggedinuser = username;
}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StompProtocol::HandleExit(std::string gameName) {
    // Build the UNSUBSCRIBE frame
    std::string id = gameSubscriptions.at(gameName);//put the channels id
    std::string receipt = std::to_string(this->receiptId++);
    std::string frame = "UNSUBSCRIBE\nid:" + id + "\nreceipt:" + receipt + "\n\n\0";

    // Send the frame to the server
    if (!Handler.sendFrameAscii(frame, '\0')) {
        std::cout << "Error sending UNSUBSCRIBE frame" << std::endl;
        return;
    }

    gameSubscriptions.erase(gameName);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StompProtocol::HandleLogout() {
    // Build the DISCONNECT frame
    std::string receipt = std::to_string(this->receiptId++);
    std::string frame = "DISCONNECT\nreceipt:" + receipt + "\n\n\0";

    // Send the frame to the server
    if (!Handler.sendFrameAscii(frame, '\0')) {
        std::cout << "Error sending DISCONNECT frame" << std::endl;
        return;
    }

    gameSubscriptions.clear();
    FirstUser=false;
     logout=true;
     notlogged=true;
     shouldClose = true;
     //Handler.close();

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StompProtocol::HandleReport(std::string file){
  //parce the provided file

  names_and_events nne = parseEventsFile(file);
  std::vector<Event> events(nne.events);
  std::string destination = nne.team_a_name + "_" + nne.team_b_name;
  std::vector<Event> gameUpdateForsummary;

  // handle the events in the the provided file
  while (!events.empty()) {
    Event removed_event = *events.begin();
    events.erase(events.begin());
    gameUpdateForsummary.push_back(removed_event);
    this->addToMap(currentloggedinuser, gameUpdateForsummary);
    std::string message = this->covertEventToString(removed_event);

    // Build the SEND frame
    std::string frame = "SEND\ndestination:/" + destination + "\n\n" + message + "\n\n\0";

    // Send the frame to the server
    if (!Handler.sendFrameAscii(frame, '\0')) {
        std::cout << "Error sending SEND frame" << std::endl;
        return;
    }
  }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StompProtocol::HandleSummary(std::string game_name, std::string user, std::string file){
  std::string team_a_name, team_b_name;
  std::string separator = "_";
  // get each team name in a string alone (without the ' _ ' between them) from the game_name
  std::size_t pos = game_name.find(separator);
  team_a_name = game_name.substr(0, pos);
  team_b_name = game_name.substr(pos + 1);

  std::string summary = team_a_name + " vs " + team_b_name +"\n" ;
  summary +=  "Game stats :\n";          
  summary +=  "General stats :\n";
  // the case if there ISN'T any file recieved - get the reports done by the provided user
  if(!file.empty()){  
    if(gameSubscriptions.count(game_name) > 0){
      std::ofstream myfile;
      myfile.open (file);
      summary += dealingWithSummary1(team_a_name,team_b_name,getfromMap(user));
      
      myfile << summary;
      myfile.close();
      //std::cout <<  summary << std::endl;
    }
    else{
      std::cout << "you are not subscribed to the game you asked a summary for, or you have not made any reports to summarize\n" << std::endl;
    }
  }
/*
  //the case if there IS a file recieved
  else {
    names_and_events nne = parseEventsFile(file);
    std::vector<Event> events(nne.events);
    if(gameSubscriptions.count(game_name) > 0){
      summary += dealingWithSummary2(team_a_name,team_b_name,events);
      std::cout <<  summary << std::endl;
    }
    else {
      std::cout << "you are not subscribed to the game you asked a summary for\n" << std::endl;
    }
  }*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// function to convert event to string
std::string StompProtocol::covertEventToString(Event evnt)
{
    std::string team_a_name = evnt.get_team_a_name();
    std::string team_b_name = evnt.get_team_b_name();
    std::string name = evnt.get_name();
    int intTime = evnt.get_time();
    std::string time = std::to_string(intTime);
    std::map<std::string, std::string> game_updates = evnt.get_game_updates();

    std::map<std::string, std::string> team_a_updates = evnt.get_team_a_updates();
    std::map<std::string, std::string> team_b_updates = evnt.get_team_b_updates();
    std::string discription = evnt.get_discription();

  std::string message = "user:" + currentloggedinuser + "\n";
  message += "team a:" + team_a_name + "\n";
  message += "team b:" + team_b_name + "\n";
  message += "event name:" + name + "\n";
  message += "time:" + time + "\n";
  message += "general game updates :\n";
  for (auto& update : game_updates) {
if (GeneralGameUpdates.count(update.first) > 0) {
    GeneralGameUpdates.erase(update.first);
    GeneralGameUpdates[update.first] = update.second;
  }
  // new user made a report
  else{
    GeneralGameUpdates[update.first] = update.second;
  }
 
 
  message += update.first + ": " + update.second + "\n";
  }
  message += "team a updates :\n";
  for (auto& update : team_a_updates) {
  message += update.first + ": " + update.second + "\n";
  }
  message += "team b updates :\n";
  for (auto& update : team_b_updates) {
  message += update.first + ": " + update.second + "\n";
  }
  message += "discription:" + discription;
  return message;
}

// function to create the continuation of the summary
std::string StompProtocol::dealingWithSummary1(std::string team_a_name, std::string team_b_name, std::vector<Event> gameUpdateForsummary)
{
  std::string summary;
  //get last general game updates
  for (auto& update : GeneralGameUpdates){
    std:: string updates = update.first + ": " + update.second;
    summary += updates + "\n";
  }
  
  // get team a updates
  summary +=  team_a_name + "stats :" + "\n"; 
  for (unsigned int i = 0; i<gameUpdateForsummary.size(); i++){
  std::map<std::string, std::string> get_team_a_updates = gameUpdateForsummary[i].get_team_a_updates();
  for (auto& update : get_team_a_updates) {
  summary += update.first + ": " + update.second + "\n";
  }
  }
  
  // get team b updates
  summary +=  team_b_name + " stats :" + "\n";
  for (unsigned int i = 0; i<gameUpdateForsummary.size(); i++){
    std::map<std::string, std::string> get_team_b_updates = gameUpdateForsummary[i].get_team_b_updates();
    for (auto& update : get_team_b_updates) {
     summary += update.first + ": " + update.second + "\n";
    }
  }

  // get game events
  summary +=  "Game event reports :\n"; 
  for(unsigned int i = 0; i<gameUpdateForsummary.size(); i++){
    int intTime = gameUpdateForsummary[i].get_time();
    std::string time = std::to_string(intTime);
    summary += time;
    summary += " - ";
    std::string name = gameUpdateForsummary[i].get_name();
    summary += name;
    std::string discription = gameUpdateForsummary[i].get_discription();
    summary += ":\n\n";
    summary += discription;
    summary += "\n\n";
  }
  return summary;

}

std::string StompProtocol::dealingWithSummary2(std::string team_a_name, std::string team_b_name, std::vector<Event> gameUpdateForsummary)
{
  std::string summary; 
  std::unordered_map< std::string, std::string> GeneralUpdates;

 for (unsigned int i = 0; i<gameUpdateForsummary.size(); i++){

    std::map<std::string, std::string> get_game_updates = gameUpdateForsummary[i].get_game_updates();
   for (auto& update : get_game_updates) {
    std::string hi = update.first + ": " + update.second + "\n";
    if (GeneralUpdates.count(update.first) > 0) {

     GeneralUpdates.erase(update.first);
     GeneralUpdates[update.first] = update.second;
    }
    else{
     GeneralUpdates[update.first] = update.second;
    }
   }
  }

    //get last general game updates
    for (auto& update : GeneralUpdates){
     std:: string updates = update.first + ": " + update.second;
     summary += updates + "\n";
    }


  // get team a updates
  summary +=  team_a_name + "stats :" + "\n"; 
  for (unsigned int i = 0; i<gameUpdateForsummary.size(); i++){
  std::map<std::string, std::string> get_team_a_updates = gameUpdateForsummary[i].get_team_a_updates();
  for (auto& update : get_team_a_updates) {
  summary += update.first + ": " + update.second + "\n";
  }
  }
  
  // get team b updates
  summary +=  team_b_name + " stats :" + "\n";
  for (unsigned int i = 0; i<gameUpdateForsummary.size(); i++){
    std::map<std::string, std::string> get_team_b_updates = gameUpdateForsummary[i].get_team_b_updates();
    for (auto& update : get_team_b_updates) {
     summary += update.first + ": " + update.second + "\n";
    }
  }

  // get game events
  summary +=  "Game event reports :\n"; 
  for(unsigned int i = 0; i<gameUpdateForsummary.size(); i++){
    int intTime = gameUpdateForsummary[i].get_time();
    std::string time = std::to_string(intTime);
    summary += time;
    summary += " - ";
    std::string name = gameUpdateForsummary[i].get_name();
    summary += name;
    std::string discription = gameUpdateForsummary[i].get_discription();
    summary += ":\n\n";
    summary += discription;
    summary += "\n\n";
  }
  return summary;
}


// adds new events to the user
void StompProtocol::addToMap(std::string user, std::vector<Event> gameUpdate)
{
  // user already made reports in the past
  if (userEvents.count(user) > 0) {
    userEvents[user].push_back(gameUpdate.back());// push the last event (that is in gameUpdate vector), since it's the new event the method got called for
  }
  // new user made a report
  else{
    userEvents[user] = gameUpdate;
  }
}

// returns the events made by the provided user
std::vector<Event> StompProtocol::getfromMap(std::string user)
{
  std::vector<Event> updates;
  if (userEvents.count(user) > 0) {
   updates = userEvents[user];
  }
  return updates;
}

