#include "User.hpp"

User::User(void)
{
	std::cout << "User constructor called" << std::endl;
}

User::User(Server* server, int userSocket)
	: _server(server)
	, listenSocket(userSocket)
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
	close(listenSocket);
}

User&	User::operator= (const User& copy)
{
	std::cout << "User assignment operator called" << std::endl;
	if (this != &copy)
	{
		this->_server = copy._server;
		this->listenSocket = copy.listenSocket;
	}
	return (*this);
}

bool    User::operator==(const User user)
{
    return this->listenSocket == user.getSocket();
}

int User::getSocket() const
{
    return listenSocket;
}

void    User::quitServer()
{

}

bool    checkChannelExists(std::string chaname)
{
//   for (std::vector<Channel>::iterator it = Server::_channels.begin() ; it != Server::_channels.end(); ++it)
//   {
//     // if (it->name == chaname)
//     // {
//     //     it->users.push_back()
//     // }
//   }
    return true;
}

void User::says(std::string message)
{
    std::cout << this->getSocket() << std::endl;
    std::vector<std::string>    div = split(message);

    if (div[0] == "JOIN")
    {
        std::cout << "Joining " << div[1] << std::endl;

    }
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

