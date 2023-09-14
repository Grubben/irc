#include "Server.hpp"

void Server::execute(std::list<ServerMessage> messageList)
{
	std::list<ServerMessage>::iterator it = messageList.begin();
    _stop = false;
    for (; it != messageList.end() && _stop == false; it++)
	{
		std::string command = it->getCommand();
		if (_commandMap.find(command) != _commandMap.end())
		{
			(this->*_commandMap[command])(*it);
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

// Done
void Server::nick(ServerMessage serverMessage)
{
    if (serverMessage.getParams().size() > 1)
    {
        // error too many params
        _stop = true;
        return;
    }
    else if (serverMessage.getParams().size() < 1)
    {
        // error too few params
        _stop = true;
        return;
    }
    if (serverMessage.getParams()[0].size() > 10)
    {
        // error nickname too long
        _stop = true;
        return;
    }
    else if (serverMessage.getParams()[0].size() < 4)
    {
        // error nickname too short
        _stop = true;
        return;
    }

    std::map<int, User>::iterator it = _users.begin();
    if(serverMessage.getParams()[0].find_first_not_of(ACCEPTED_CHARS_NAME) != std::string::npos)
    {
        // error invalid chars
        _stop = true;
        return;
    }

    // Is nick in use
    for (; it != _users.end(); it++)
    {
        if (it->second.getNickname() == serverMessage.getParams()[0])
        {
            // error nickname in use
            _stop = true;
            return;
        }
    }
    
    // If not in use, set nickname
    _users[serverMessage.getSocket()].setNickname(serverMessage.getParams()[0]);
    std::cout << "new nickname: " << _users[serverMessage.getSocket()].getNickname() << std::endl;
}

void Server::user(ServerMessage serverMessage)
{
    if (serverMessage.getParams().size() != 4)
    {
        // error wrong num of params means that exists a space in the username or no username
        _stop = true;
        return;
    }
    
    if (serverMessage.getParams()[0].size() > 10)
    {
        // error username too long
        _stop = true;
        return;
    }
    else if (serverMessage.getParams()[0].size() < 4)
    {
        // error username too short
        _stop = true;
        return;
    }

    std::map<int, User>::iterator it = _users.begin();
    if(serverMessage.getParams()[0].find_first_not_of(ACCEPTED_CHARS_NAME) != std::string::npos)
    {
        // error invalid chars
        _stop = true;
        return;
    }

    // Is user in use
    for (; it != _users.end(); it++)
    {
        if (it->second.getUsername() == serverMessage.getParams()[0])
        {
            // error username in use
            _stop = true;
            return;
        }
    }
    
    // If not in use, set username
    _users[serverMessage.getSocket()].setUsername(serverMessage.getParams()[0]);
    std::cout << "new username: " << _users[serverMessage.getSocket()].getUsername() << std::endl;
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

