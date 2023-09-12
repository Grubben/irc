#pragma once
#include <iostream>
#include <list>

class Server;
class User;

class Channel
{
private:
	Server*				_server;
	std::list<User*>	_chanusers;
public:
	Channel(std::string name);
	Channel(const Channel& copy);
	~Channel(void);

	Channel&	operator= (const Channel& copy);


	std::string			_name;

	/*	API	*/
	void	userAdd(User& user);
	int		userRemove(User& user);

	void	usersDrop();
};

Channel& getChannel(std::list<Channel*>& channels, std::string chaname);
