#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once
#include <iostream>
#include <list>
#include "Exceptions.hpp"
#include "Server.hpp"

class Channel
{
private:
	Server&					_server;
	const std::string		_name;
	std::string				_topic;

	std::map<int, User*>	_chanusers;

	void	usersDrop(); //Not API: only used internally
public:
	Channel(Server& server, std::string name);
	Channel(const Channel& copy);
	Channel& operator=(const Channel& copy);
	~Channel();

	/*	Getters & Setters	*/
	const std::string&	getName(void) const { return (_name); }
	std::map<int,User*>&	getUsers(void) { return _chanusers; }
	std::string	getUsersString(void) {
		std::string users = "";
		for (std::map<int,User*>::iterator it = _chanusers.begin(); it != _chanusers.end(); it++)
		{
			std::string tmp = it->second->getNickname();
			users += " " + tmp;
		}
		return users;
	}
	
	// void 				setName(std::string name) { _name = name; } //Warning: should delete. Name cannot be changed
	const std::string&	getTopic() const { return _topic; }

	/*	API	*/
	void	userAdd(User& user);
	int		userRemove(User& user);
};


#endif