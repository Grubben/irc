#include "User.hpp"

User::User(Server* server, int userSocket)
{
    _server = server;
    _socket = userSocket;
}

User::~User()
{
    close(_socket);
    _socket = 0;
}

int User::getSocket() const
{
    return _socket;
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

bool    User::operator==(const User user)
{
    return this->_socket == user.getSocket();
}
