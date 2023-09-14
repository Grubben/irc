#include "Channel.hpp"

Channel::Channel(Server& server, std::string name)
    : _server(server)
    , _name(name)
{
	std::cout << "Channel constructor called" << std::endl;
    _chanusers.clear();
}

Channel::Channel(const Channel& copy)
    : _server(copy._server)
    , _name(copy._name)
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
    _chanusers.insert(std::pair<int,User&>(user.getSocket(), user));
}

int    Channel::userRemove(User& user)
{
    std::map<int,User&>::iterator search = _chanusers.find(user.getSocket());
    _chanusers.erase(search);

    std::cout << "Channel with users: " << _chanusers.size() << std::endl;

    return _chanusers.size();   
}

void    Channel::usersDrop()
{
    //TODO: Refactor!
    // while (_chanusers.size() > 0)
    // {
    //     User*   useri = _chanusers.back();
    //     useri->channelPart(_server, *this);
    //     _chanusers.pop_back();
    // }
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
