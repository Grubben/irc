#include "User.hpp"
#include "Server.hpp"

User::User(Server& server, int userSocket)
	: _server(server)
	, _socket(userSocket)
{
	std::cout << "User constructor called" << std::endl;
    _isLoggedIn = false;
    _isOperator = false;
}

User::User(const User& copy)
	: _server(copy._server)
	, _socket(copy._socket)
{
	std::cout << "User copy constructor called" << std::endl;
	*this = copy;
}

User::~User(void)
{
	std::cout << "User destructor called" << std::endl;

	this->channelsDrop();

	// close(_socket); //Warning: Do NOT uncomment!!!
}

User&	User::operator= (const User& copy)
{
	std::cout << "User assignment operator called" << std::endl;
	if (this != &copy)
	{}
	return (*this);
}

void User::says(std::string message)
{
    std::list<ServerMessage> messageList = loadMessageIntoList(message);
    
    _server.execute(messageList);
}

void	User::channelJoin(Channel& channel)
{
	_channels.insert(std::pair<std::string,Channel&>(channel.getName(), channel));
}

void	User::channelPart(std::string chaname)
{
	std::map<std::string, Channel&>::iterator search = _channels.find(chaname);
	_channels.erase(search);
}

void	User::channelsDrop()
{
	for (std::map<std::string, Channel&>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		it->second.userRemove(*this);
		channelPart(it->first);
	}
}