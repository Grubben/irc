#include "Channel.hpp"
#include "Exceptions.hpp"
#include "Server.hpp"

Channel::Channel(std::string name)
    : _name(name)
{
	std::cout << "Channel constructor called" << std::endl;
}

Channel::Channel(const Channel& copy)
{
	std::cout << "Channel copy constructor called" << std::endl;
	*this = copy;
}

Channel::~Channel(void)
{
	std::cout << "Channel destructor called" << std::endl;
}

Channel&	Channel::operator= (const Channel& copy)
{
	std::cout << "Channel assignment operator called" << std::endl;
	if (this != &copy)
	{}
	return (*this);
}

void    Channel::userAdd(User& user)
{
    _chanusers.push_back(&user);
}

void    Channel::userRemove(User& user)
{
    _chanusers.remove(&user);
    if (_chanusers.size() == 0)
    {
        _server->destroyChannel(*this);
    }

}

void    Channel::usersDrop()
{
    User*   useri;

    for (User* useri = _chanusers.pop_back(); _chanusers.size > 0; useri = _chanusers.pop_back())
    {
        useri->channelLeave(_server, *this);
    }
}

Channel& getChannel(std::list<Channel*>& channels, std::string chaname)
{
    for (std::list<Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        if ((*it)->_name == chaname)
        {
            return *(*it);
        }
    }
    throw ChannelUnableToFind();
}