#include "Server.hpp"

int capabilityNegotiation();
int connectionRegistration(ServerMessage serverMessage, Server* server);
int miscellaneousMessages(ServerMessage serverMessage, Server *server);
int userBasedQueries(ServerMessage serverMessage, Server *server);
int sendingMessages(ServerMessage serverMessage, Server *server);
int channelOperations(ServerMessage serverMessage, Server *server);

void Server::messageHandler(std::string message)
{
    std::vector<std::string> messageBuffer = split(message, "\r\n");
    for (int i = 0; i < static_cast<int>(messageBuffer.size()); i++)
    {
        std::vector<std::string> commandVector = split(messageBuffer[i], " ");
        ServerMessage serverMessage(commandVector);
        std::cout << "command: " << serverMessage.getCommand() << std::endl;
        
        //  In this order:
        // This evaluates all the Capability negotiation
        // This evaluates all the 4.1 commands of RFC 1459
        // This evaluates all the 4.2 commands of RFC 1459
        // This evaluates all the 4.4 commands of RFC 1459
        // This evaluates all the 4.5 commands of RFC 1459
        // This evaluates all the 4.6 commands of RFC 1459
        if (!(capabilityNegotiation() || \
            connectionRegistration(serverMessage, this) || \
            channelOperations(serverMessage, this) || \
            sendingMessages(serverMessage, this) || \
            userBasedQueries(serverMessage, this) || \
            miscellaneousMessages(serverMessage, this)))
        {
            std::cout << "command not found" << std::endl;
        }
    }
}

int capabilityNegotiation()
{
    // The server does not provide capability negotiation I THINK
    // https://ircv3.net/specs/extensions/capability-negotiation.html
    return (0);
}

int miscellaneousMessages(ServerMessage serverMessage, Server *server)
{
    if (serverMessage.getCommand() == "KILL")
    {
        std::cout << "servlist command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "ERROR")
    {
        std::cout << "squery command" << std::endl;
        return (1);
    }
    return (0);
}

int userBasedQueries(ServerMessage serverMessage, Server *server)
{
    if (serverMessage.getCommand() == "WHO")
    {
        std::cout << "who command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "WHOIS")
    {
        std::cout << "whois command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "WHOWAS")
    {
        std::cout << "whowas command" << std::endl;
        return (1);
    }
    return (0);
}

int sendingMessages(ServerMessage serverMessage, Server *server)
{
    if (serverMessage.getCommand() == "PRIVMSG")
    {
        std::cout << "privmsg command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "NOTICE")
    {
        std::cout << "notice command" << std::endl;
        return (1);
    }
    return (0);
}

int channelOperations(ServerMessage serverMessage, Server *server)
{
    if (serverMessage.getCommand() == "JOIN")
    {
        std::cout << "join command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "PART")
    {
        std::cout << "part command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "MODE")
    {
        std::cout << "mode command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "TOPIC")
    {
        std::cout << "topic command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "NAMES")
    {
        std::cout << "names command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "LIST")
    {
        std::cout << "list command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "INVITE")
    {
        std::cout << "invite command" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "KICK")
    {
        std::cout << "kick command" << std::endl;
        return (1);
    }
    return (0);
}

void passwordVerification(std::string password, Server* server)
{
    if (password == server->getEnvironment().getPassword())
    {
        sendNumericResponse(server->getClientFDs().back(), 376, "Password match! Welcome to the server!");
        std::cout << "password verified" << std::endl;
    }
    else
        std::cout << "password not verified" << std::endl;
}

int connectionRegistration(ServerMessage serverMessage, Server* server)
{
    if (serverMessage.getCommand() == "PASS")
    {
        std::cout << "password verification and load" << std::endl;
        passwordVerification(serverMessage.getParams().back(), server);
        return (1);
    }
    else if (serverMessage.getCommand() == "NICK")
    {
        std::cout << "nick verification and load" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "USER")
    {
        std::cout << "user verification and load" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "OPER")
    {
        std::cout << "oper verification and load" << std::endl;
        return (1);
    }
    else if (serverMessage.getCommand() == "QUIT")
    {
        std::cout << "quit verification and load" << std::endl;
        return (1);
    }
    return (0);
}
