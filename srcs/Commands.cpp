#include "Server.hpp"

void Server::execute(std::list<ServerMessage> messageList)
{
	std::list<ServerMessage>::iterator it = messageList.begin();
	for (; it != messageList.end(); it++)
	{
		std::string command = it->getCommand();
		if (_commandMap.find(command) != _commandMap.end())
		{
			(this->*_commandMap[command])(messageList);
		}
		else
		{
			std::cout << "Command not found" << std::endl; // needs to send numeric?
		}
	}
}
void Server::cap(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    // The server does not provide capability negotiation I THINK
    //  OR maybe this is needed to do the file transfer, need to check
    // https://ircv3.net/specs/extensions/capability-negotiation.html
    std::cout << "cap command" << std::endl;
}

void Server::pass(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    std::cout << "pass command" << std::endl;
}

void Server::nick(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    std::cout << "nick command" << std::endl;
}

void Server::user(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    std::cout << "user command" << std::endl;
}

void Server::oper(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    std::cout << "oper command" << std::endl;
}

void Server::quit(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    std::cout << "quit command" << std::endl;
}

void Server::join(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    std::cout << "join command" << std::endl;
}

void Server::part(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    std::cout << "part command" << std::endl;
}

void Server::mode(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    std::cout << "mode command" << std::endl;
}

void Server::topic(std::list<ServerMessage> ServerMessage)
{
    (void) ServerMessage;
    std::cout << "topic command" << std::endl;
}

