#include "../include/ClientInput.h"
extern bool shouldClose;
extern bool logout;
extern bool FirstUser;
//extern volatile bool recieve;
extern volatile bool notlogged;

ClientInput::ClientInput(ConnectionHandler &Handler, StompProtocol &protocol) : Handler(Handler), protocol(protocol) {}

void ClientInput::run()
{
  std::string input;
  
  while (!shouldClose)
  {
    getline(std::cin, input);
    if ((!logout) & (input != ""))
    {
      int firstSpacePos = input.find(' '); // finding place of first word(command)
      std::string command = input.substr(0, firstSpacePos);
      if (command == "login")
      {
        notlogged = false;
        loginParameters(input);
      }
      else if (command == "join")
      {
        joinParameters(input);
      }
      else if (command == "exit")
      {
        exitParameters(input);
      }
      else if (command == "logout")
      {
        logoutParameters(input);
      }
      // login 127.0.0.1:7777 user pass
      else if (command == "report")
      {
      reportParameters(input);
      }
      else if (command == "summary")
      {
       summaryParameters(input);
      }
    }
    if(logout&(input!="")){
      int firstSpacePos = input.find(' '); // finding place of first word(command)
      std::string command = input.substr(0, firstSpacePos);
      if(command=="login"){
        loginParameters(input);
        logout=false;
        notlogged = false;
      }
    }
  }
}
// parses login components and sends them to protocol
void ClientInput::loginParameters(std::string input)
{

  int firstSpacePos = input.find(' ');
  int secondSpacePos = input.find(' ', firstSpacePos + 1); // finds the host and port place
  std::string hostPort = input.substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);
  int thirdSpacePos = input.find(' ', secondSpacePos + 1);
  std::string username = input.substr(secondSpacePos + 1, thirdSpacePos - secondSpacePos - 1);
  std::string password = input.substr(thirdSpacePos + 1);
  protocol.HandleLogin(hostPort, username, password);

}
// parses join components and sends them to protocol
void ClientInput::joinParameters(std::string input)
{
  int firstSpacePos = input.find(' ');
  std::string gameName = input.substr(firstSpacePos + 1);
  protocol.HandleJoin(gameName);
}
// parses exit components and sends them to protocol
void ClientInput::exitParameters(std::string input)
{
  int firstSpacePos = input.find(' ');
  std::string gameName = input.substr(firstSpacePos + 1);
  protocol.HandleExit(gameName);
}
// parses logout components and sends them to protocol
void ClientInput::logoutParameters(std::string input)
{
  // there are no parameters for logout other than the command so we just handle it
  protocol.HandleLogout();
}
// parses summary components and sends them to protocol
void ClientInput::reportParameters(std::string input)
{
  int firstSpacePos = input.find(' ');
  std::string file = input.substr(firstSpacePos + 1);
  protocol.HandleReport(file);
}
// parses summary components and sends them to protocol
void ClientInput::summaryParameters(std::string input)
{
 int firstSpacePos = input.find(' ');
  int secondSpacePos = input.find(' ', firstSpacePos + 1); // finds the name of the game
  std::string game_name = input.substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);
  std::string rest = input.substr(secondSpacePos+1);
  int thirdSpacePos = rest.find(' ');
  std::string username;
  std::string file;
  //the case if there IS a file recieved
  if(thirdSpacePos != -1){
    username = rest.substr(0, thirdSpacePos);
    file = rest.substr(thirdSpacePos+1);
  }
  // the case if there ISN'T any file recieved
  else{
    username = rest;
 }
  protocol.HandleSummary(game_name, username, file);
}
