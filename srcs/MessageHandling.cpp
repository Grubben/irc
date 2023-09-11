#include "Server.hpp"

void User::messageHandler(std::string message, Server *server)
{
    std::vector<std::string> messageBuffer = split(message, "\r\n");
    std::list<ServerMessage> messageList;
    for (int i = 0; i < static_cast<int>(messageBuffer.size()); i++)
    {
        messageList.push_back(ServerMessage(split(messageBuffer[i], " ")));
    }
    while (!messageList.empty())
    {
        std::cout << "command: " << messageList.front().getCommand() << std::endl;

        //  In this order:
        // This evaluates all the Capability negotiation
        // This evaluates all the 4.1 commands of RFC 1459
        // This evaluates all the 4.2 commands of RFC 1459
        // This evaluates all the 4.4 commands of RFC 1459
        // This evaluates all the 4.5 commands of RFC 1459
        // This evaluates all the 4.6 commands of RFC 1459
        if (!(capabilityNegotiation() || \
            connectionRegistration(messageList, server) || \
            channelOperations(messageList, server) || \
            sendingMessages(messageList, server) || \
            userBasedQueries(messageList, server) || \
            miscellaneousMessages(messageList, server)))
        {
            std::cout << "command not found" << std::endl;
        }
        messageList.pop_front();
    }
}

int User::capabilityNegotiation()
{
    // The server does not provide capability negotiation I THINK
    // https://ircv3.net/specs/extensions/capability-negotiation.html
    return (0);
}

void credentialsVerification(std::string password, Server* server)
{
    if (password == server->getEnvironment().getPassword())
    {
        
        std::cout << "password verified" << std::endl;
    }
    else
        std::cout << "password not verified" << std::endl;
}

int User::connectionRegistration(std::list<ServerMessage> messageList, Server* server)
{
    (void) server;
    if (messageList.front().getCommand() == "PASS")
    {
        std::cout << "password verification and load" << std::endl;
        credentialsVerification(messageList.front().getParams().back(), server);
        return (1);
    }
    else if (messageList.front().getCommand() == "NICK")
    {
        // must give an error due to not providing a password
        std::cout << "nick command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "USER")
    {
        // must give an error due to not providing a password
        std::cout << "user command" << std::endl;
        return (1);
    }
    
    if (messageList.front().getCommand() == "OPER")
    {
        std::cout << "oper command" << std::endl;
        return (1);
    }
    if (messageList.front().getCommand() == "QUIT")
    {
        std::cout << "quit command" << std::endl;
        return (1);
    }
    return (0);
}

int User::channelOperations(std::list<ServerMessage> messageList, Server *server)
{
    (void) server;
    if (messageList.front().getCommand() == "JOIN")
    {
        std::cout << "join command" << std::endl;

        // server->channelJoin(messageList.front().getParams(2), this);

        // sendNumericResponse(this->_socket)
        return (1);
    }
    else if (messageList.front().getCommand() == "PART")
    {
        std::cout << "part command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "MODE")
    {
        std::cout << "mode command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "TOPIC")
    {
        std::cout << "topic command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "NAMES")
    {
        std::cout << "names command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "LIST")
    {
        std::cout << "list command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "INVITE")
    {
        std::cout << "invite command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "KICK")
    {
        std::cout << "kick command" << std::endl;
        return (1);
    }
    return (0);
}

int User::sendingMessages(std::list<ServerMessage> messageList, Server *server)
{
    (void) server;
    if (messageList.front().getCommand() == "PRIVMSG")
    {
        std::cout << "privmsg command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "NOTICE")
    {
        std::cout << "notice command" << std::endl;
        return (1);
    }
    return (0);
}

int User::userBasedQueries(std::list<ServerMessage> messageList, Server *server)
{
    (void) server;
    if (messageList.front().getCommand() == "WHO")
    {
        std::cout << "who command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "WHOIS")
    {
        std::cout << "whois command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "WHOWAS")
    {
        std::cout << "whowas command" << std::endl;
        return (1);
    }
    return (0);
}

int User::miscellaneousMessages(std::list<ServerMessage> messageList, Server *server)
{
    (void) server;
    if (messageList.front().getCommand() == "KILL")
    {
        std::cout << "servlist command" << std::endl;
        return (1);
    }
    else if (messageList.front().getCommand() == "ERROR")
    {
        std::cout << "squery command" << std::endl;
        return (1);
    }
    return (0);
}

