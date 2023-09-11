#ifndef SERVERMESSAGE_HPP
#define SERVERMESSAGE_HPP

#include <string>
#include <vector>
#include <iostream>

class ServerMessage
{
private:
    std::string _prefix;
    std::string _command;
    std::vector<std::string> _params;

public:
    ServerMessage();
    ServerMessage(std::vector<std::string> commandVector);
    ~ServerMessage();

    std::string getPrefix() const;
    std::string getCommand() const;
    std::vector<std::string> getParams() const;  

    void outputPrompt();
};

#endif