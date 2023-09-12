#include "User.hpp"
#include "Server.hpp"

// User::User(void)
// {
// 	std::cout << "User constructor called" << std::endl;
// }

User::User(Server* server, int userSocket)
	: _server(server)
	, _socket(userSocket)
{
	std::cout << "User constructor called" << std::endl;
}

User::User(const User& copy)
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
		this->_socket = copy._socket;
	}
	return (*this);
}

bool    User::operator==(const int sock)
{
    return this->_socket == sock;
}



int User::getSocket() const
{
    return _socket;
}

void    User::quitServer()
{

}

// bool    checkChannelExists(std::string chaname)
// {
//   for (std::vector<Channel>::iterator it = Server::_channels.begin() ; it != Server::_channels.end(); ++it)
//   {
//     // if (it->name == chaname)
//     // {
//     //     it->users.push_back()
//     // }
//   }
//     return true;
// }

void User::says(std::string message, Server *server)
{
    messageHandler(message, server);
}

// void    Server::broadcast(const std::string msg)
// {
//     std::vector<User>::iterator it = users.begin();
//     for (; it != users.end(); it++)
//     {
//         // if (*it != *it)
//         if (send(*it, msg.c_str(), msg.length(), 0) == -1)
//             throw SocketSendingError();
//     }
// }

void	User::channelJoin(Server* server, std::string chaname)
{
	_channels.push_back( server->addUserToChannel(*this, chaname) );
}

// void	User::channelSubscribe(Server* server, Channel& channel)
// {
// 	server->addUserToChannel(*this, channel);
// }

void	User::channelLeave(Server* server, Channel& channel)
{
	server->rmUserFromChannel(*this, channel);
}

void	User::channelsDrop()
{
	for (std::list<Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		_server->rmUserFromChannel(*this, **it);
	}
	_channels.clear();
}