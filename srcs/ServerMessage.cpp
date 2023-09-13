#include "ServerMessage.hpp"

ServerMessage::ServerMessage()
{
}

ServerMessage::ServerMessage(std::vector<std::string> commandVector)
{
    _command = commandVector[0];
    for (int i = 1; i < static_cast<int>(commandVector.size()); i++)
        _params.push_back(commandVector[i]);
}

void ServerMessage::outputPrompt()
{
    std::cout << "command: " << _command << std::endl;
    std::cout << "params: " << std::endl;
    for (int i = 0; i < static_cast<int>(_params.size()); i++)
        std::cout << "[" << i << "]" << " = " << _params[i] << std::endl;
}

ServerMessage::~ServerMessage()
{
}
