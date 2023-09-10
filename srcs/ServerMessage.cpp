#include "ServerMessage.hpp"

ServerMessage::ServerMessage()
{
}

ServerMessage::ServerMessage(std::vector<std::string> commandVector)
{
    int i = 0;

    if (commandVector[0][0] == ':')
    {
        _prefix = commandVector[0];
        i++;
    }
    else if (commandVector[0][0] != ':')
        _prefix = "";
    _command = commandVector[i++];
    for (; i < static_cast<int>(commandVector.size()); i++)
        _params.push_back(commandVector[i]);
}

void ServerMessage::outputPrompt()
{
    std::cout << "prefix: " << _prefix << std::endl;
    std::cout << "command: " << _command << std::endl;
    std::cout << "params: " << std::endl;
    for (int i = 0; i < static_cast<int>(_params.size()); i++)
        std::cout << "[" << i << "]" << " = " << _params[i] << std::endl;
}

ServerMessage::~ServerMessage()
{
}
