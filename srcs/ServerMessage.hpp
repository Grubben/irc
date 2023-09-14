#ifndef SERVERMESSAGE_HPP
#define SERVERMESSAGE_HPP

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "Utils.hpp"

class ServerMessage
{
private:
    std::string                 _command;
    std::vector<std::string>    _params;
    int                         _socket; // socket is the user who sent the message

public:
    ServerMessage();
    ServerMessage(std::vector<std::string> commandVector, int socket);
    ~ServerMessage();

    std::string getCommand() const              { return _command; }
    std::vector<std::string> getParams() const  { return _params; }
    int getSocket() const                       { return _socket; }
    void setSocket(int socket)    { _socket = socket; }
    
    static std::list<ServerMessage>    loadMessageIntoList(std::string message, int socket);
    void outputPrompt();
};

#endif