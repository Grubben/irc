#include "Server.hpp"

void Server::execute(std::list<ServerMessage> messageList)
{
	std::list<ServerMessage>::iterator it = messageList.begin();
	for (; it != messageList.end(); it++)
	{
		std::string command = it->getCommand();
		if (_commandMap.find(command) != _commandMap.end())
		{
			(this->*_commandMap[command])(messageList.front());
		}
		else
		{
			std::cout << "Command not found" << std::endl; // needs to send numeric?
		}
	}
}

void Server::cap(ServerMessage serverMessage)
{
    // The server does not provide capability negotiation I THINK
    //  OR maybe this is needed to do the file transfer, need to check
    // https://ircv3.net/specs/extensions/capability-negotiation.html
    std::cout << "cap command" << std::endl;
}

void Server::pass(ServerMessage serverMessage)
{
    std::cout << "pass command" << std::endl;
}

// First is nick and user. If it passes through those, then it sends a pass

void Server::nick(ServerMessage serverMessage)
{
    std::cout << "nick command" << std::endl;
}

void Server::user(ServerMessage serverMessage)
{
    std::cout << "user command" << std::endl;
}

void Server::oper(ServerMessage serverMessage)
{
    std::cout << "oper command" << std::endl;
}

void Server::quit(ServerMessage serverMessage)
{
    std::cout << "quit command" << std::endl;
}

void Server::join(ServerMessage serverMessage)
{
    std::cout << "join command" << std::endl;
}

void Server::part(ServerMessage serverMessage)
{
    std::cout << "part command" << std::endl;
}

void Server::mode(ServerMessage serverMessage)
{
    std::cout << "mode command" << std::endl;
}

void Server::topic(ServerMessage serverMessage)
{
    std::cout << "topic command" << std::endl;
}

