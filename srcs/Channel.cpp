#include "Channel.hpp"

Channel::Channel(Server& server, std::string name)
    : _server(server)
    , _name(name)
{
	std::cout << "Channel constructor called" << std::endl;
    _topic = "<Default Topic>";
    _chanusers.clear();

    _isInviteOnly = false;
    _invited.clear();
    _topicRestrict = true;
    _hasPassword = false;
    _password = "";
    _userLimit = false;
    _maxUsers = 0; // if 0, no limit
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
}

Channel&	Channel::operator= (const Channel& copy)
{
	//std::cout << "Channel assignment operator called" << std::endl;
	if (this != &copy)
	{
        _topic = copy._topic;
        _chanusers = copy._chanusers;
        _isInviteOnly = copy._isInviteOnly;
        _invited = copy._invited;
        _topicRestrict = copy._topicRestrict;
        _hasPassword = copy._hasPassword;
        _password = copy._password;
        _userLimit = copy._userLimit;
        _maxUsers = copy._maxUsers;
        _operators = copy._operators;
    }
	return (*this);
}

void    Channel::userAdd(User& user)
{
    _chanusers.insert(std::pair<int, User*>(user.getSocket(), &user));
}

int    Channel::userRemove(User& user)
{
    std::map<int, User*>::iterator search = _chanusers.find(user.getSocket());
    if (search != _chanusers.end())
        _chanusers.erase(search);

    // std::cout << "Channel with users: " << _chanusers.size() << std::endl;

    return _chanusers.size();
}

bool    Channel::isUserInChannel(User& user)
{
    std::map<int, User*>::iterator  search = _chanusers.find(user.getSocket());
    return search != _chanusers.end();
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

void	Channel::broadcastMessagetoChannel(std::string message, User& user)
{
	for (std::map<int,User*>::iterator it = _chanusers.begin(); it != _chanusers.end(); it++)
	{
		if (it->second->getSocket() != user.getSocket())
			_server.sendSuccessMessage(it->second->getSocket(), message, "");
	}
};