#include "User.hpp"
#include "Server.hpp"

User::User(Server* server, int userSocket)
	: _server(server)
	, _socket(userSocket)
{
	std::cout << "User constructor called" << std::endl;
    _isLoggedIn = false;
    _isOperator = false;

    /* commands */
	_commandMap["CAP"] = &User::cap;
    _commandMap["PASS"] = &User::pass;
	_commandMap["NICK"] = &User::nick;
	_commandMap["USER"] = &User::user;
	_commandMap["OPER"] = &User::oper;
	_commandMap["QUIT"] = &User::quit;
	_commandMap["JOIN"] = &User::join;
	_commandMap["PART"] = &User::part;
	_commandMap["MODE"] = &User::mode;
	_commandMap["TOPIC"] = &User::topic;
}

User::User(const User& copy)
	: _socket(copy._socket)
{
	std::cout << "User copy constructor called" << std::endl;
	*this = copy;
}

User::~User(void)
{
	std::cout << "User destructor called" << std::endl;

	this->channelsDrop();

	close(_socket);
}

User&	User::operator= (const User& copy)
{
	std::cout << "User assignment operator called" << std::endl;
	if (this != &copy)
	{
		this->_server = copy._server;
	}
	return (*this);
}

void User::execute(std::list<ServerMessage> messageList, Server* server)
{
	std::list<ServerMessage>::iterator it = messageList.begin();
	for (; it != messageList.end(); it++)
	{
		std::string command = it->getCommand();
		if (_commandMap.find(command) != _commandMap.end())
		{
			(this->*_commandMap[command])(messageList, server);
		}
		else
		{
			std::cout << "Command not found" << std::endl; // needs to send numeric?
		}
	}
}

void User::says(std::string message, Server *server)
{
    std::list<ServerMessage> messageList = loadMessageIntoList(message);
    
    this->execute(messageList, server);
}

void	User::channelJoin(Server* server, std::string chaname)
{
	server->userAddToChannel(*this, chaname);
}

void	User::channelSubscribe(Channel* channel)
{
	_channels.push_back(channel);
}

void	User::channelPart(Server* server, Channel& channel)
{
	server->userRmFromChannel(*this, channel);
}

void	User::channelsDrop()
{
	for (std::list<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		_server->userRmFromChannel(*this, **it);
	}
}