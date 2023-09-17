#include "User.hpp"
#include "Server.hpp"

User::User() : _socket(-1) 
{}

User::User(int userSocket): _socket(userSocket)
{
    _isLoggedIn = false;
    _isOperator = false;
}

User::User(const User& copy):_socket(copy._socket)
{
	*this = copy;
}

User::~User(void)
{
	// close(_socket); //Warning: Do NOT uncomment!!!
	// this->channelsDrop();

	_isOperator = false;
	_isLoggedIn = false;
	// do these values automatically reset when the object is destroyed?
	//  Or does map keep it in the key: socketfd ??
}

User&	User::operator= (const User& copy)
{
	if (this != &copy)
	{
		_userChannels = copy._userChannels;
		_username = copy._username;
		_nickname = copy._nickname;
		_isLoggedIn = copy._isLoggedIn;
		_isOperator = copy._isOperator;
	}
	return (*this);
}

void User::says(std::string message, Server& server)
{
    std::list<ServerMessage> messageList = ServerMessage::loadMessageIntoList(message, _socket);
    
    server.execute(messageList);
}

void	User::channelJoin(Channel& channel)
{
	_userChannels.insert(std::pair<std::string, Channel*>(channel.getName(), &channel));
}

void	User::channelPart(std::string chaname)
{
	std::map<std::string, Channel*>::iterator search = _userChannels.find(chaname);
	if (search != _userChannels.end())
		_userChannels.erase(search);
}

void	User::channelsDrop()
{
	for (std::map<std::string, Channel*>::iterator it = _userChannels.begin(); it != _userChannels.end(); it++)
	{
		(*it->second).userRemove(*this);
		channelPart(it->first);
	}
}