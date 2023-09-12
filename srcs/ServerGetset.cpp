
#include "Server.hpp"


std::string ServerMessage::getPrefix() const
{
    return _prefix;
}

std::string ServerMessage::getCommand() const
{
    return _command;
}

std::vector<std::string> ServerMessage::getParams() const
{
    return _params;
}

Server::~Server()
{
    close(_listenSocket);

    //TODO: correct clean-up in case of weird stuff
    // _users.clear();  //Warning: Does not delete if there are any left
    // _channels.clear(); //Warning: Does not delete if there are any left
    //_
    
}

User&	Server::getUserBySocket(int socket)
{
    for (std::list<User*>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        if ((*it)->getSocket() == socket)
        {
            return (**it);
        }
    }
    throw SocketUnableToFindUser();
}


ServerEnvironment Server::getEnvironment() const
{
    return _environment;
}

std::list<User*> Server::getUsers() const
{
    return _users;
}

// std::vector<Channel> Server::getChannels() const
// {
//     return _channels;
// }

sockaddr_in Server::getAddress() const
{
    return _address;
}

socklen_t Server::getAddrSize() const
{
    return _addr_size;
}

int Server::getSocket() const
{
    return _listenSocket;
}

void Server::setEnvironment(ServerEnvironment environment)
{
    _environment = environment;
}

void Server::setAddress(sockaddr_in address)
{
    _address = address;
}

void Server::setAddrSize(socklen_t addrSize)
{
    _addr_size = addrSize;
}

void Server::setSocket(int socket)
{
    _listenSocket = socket;
}
