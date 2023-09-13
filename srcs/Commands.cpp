#include "User.hpp"

void User::cap(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    // The server does not provide capability negotiation I THINK
    //  OR maybe this is needed to do the file transfer, need to check
    // https://ircv3.net/specs/extensions/capability-negotiation.html
    std::cout << "cap command" << std::endl;
}

void User::pass(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    std::cout << "pass command" << std::endl;
}

void User::nick(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    std::cout << "nick command" << std::endl;
}

void User::user(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    std::cout << "user command" << std::endl;
}

void User::oper(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    std::cout << "oper command" << std::endl;
}

void User::quit(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    std::cout << "quit command" << std::endl;
}

void User::join(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    std::cout << "join command" << std::endl;
}

void User::part(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    std::cout << "part command" << std::endl;
}

void User::mode(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    std::cout << "mode command" << std::endl;
}

void User::topic(std::list<ServerMessage> ServerMessage, Server* server)
{
    (void) server;
    (void) ServerMessage;
    std::cout << "topic command" << std::endl;
}

