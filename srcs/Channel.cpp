#include "Channel.hpp"

Channel::Channel(Server* server, std::string name)
    : _server(server)
    , _name(name)
{
	std::cout << "Channel constructor called" << std::endl;
    _chanusers.clear();
}

Channel::Channel(const Channel& copy)
{
	std::cout << "Channel copy constructor called" << std::endl;
	*this = copy;
}

Channel::~Channel(void)
{
	std::cout << "Channel destructor called" << std::endl;
    usersDrop();
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
    std::cout << "Channel with users: " << _chanusers.size() << std::endl;
    if (_chanusers.size() == 0)
    {
        _server->channelDestroy(*this);
    }
}

void    Channel::usersDrop()
{
    while (_chanusers.size() > 0)
    {
        User*   useri = _chanusers.back();
        useri->channelPart(_server, *this);
        _chanusers.pop_back();
    }
}

Channel& getChannel(std::list<Channel*>& channels, std::string chaname)
{
    for (std::list<Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
    {
        if ((*it)->getName() == chaname)
        {
            return *(*it);
        }
    }
    throw ChannelUnableToFind();
}
