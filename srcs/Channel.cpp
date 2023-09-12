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

int    Channel::userRemove(User& user)
{
    _chanusers.remove(&user);
    return _chanusers.size();
}

void    Channel::usersDrop()
{
    while (_chanusers.size() > 0)
    {
        User*   useri = _chanusers.back();
        useri->channelLeave(_server, *this);
        _chanusers.pop_back();
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