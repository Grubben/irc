#include "ServerMessage.hpp"

ServerMessage::ServerMessage()
{
}

ServerMessage::ServerMessage(std::vector<std::string> commandVector, int socket)
{
    _command = toUpper(commandVector[0]);
    for (int i = 1; i < static_cast<int>(commandVector.size()); i++)
        _params.push_back(commandVector[i]);
    _socket = socket;
}

void ServerMessage::outputPrompt()
{
    std::cout << "command: " << _command << std::endl;
    std::cout << "params: ";
    int i = 0;
    for ( ; i < static_cast<int>(_params.size()); i++)
        std::cout << "[" << i << "]" << " = " << _params[i] << std::endl;
    if (i == 0)
        std::cout << "none" << std::endl;
}

ServerMessage::~ServerMessage()
{
}

std::list<ServerMessage> ServerMessage::loadMessageIntoList(std::string message, int socket)
{
    std::vector<std::string> messageBuffer = split(message, "\r\n");
    std::list<ServerMessage> messageList;
    for (int i = 0; i < static_cast<int>(messageBuffer.size()); i++)
    {
        messageList.push_back(ServerMessage(split(messageBuffer[i], " "), socket));
    }
    return (messageList);
}