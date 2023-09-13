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
	Server*				_server;
	std::string			_name;

	std::list<User*>	_chanusers;

	void	usersDrop(); //Not API: only used internally
public:
	Channel(Server* server, std::string name);
	Channel(const Channel& copy);
	Channel& operator=(const Channel& copy);
	~Channel();

	/*	Getters & Setters	*/
	const std::string&	getName(void) const { return (_name); }
	std::list<User*>&	getUsers(void) { return (_chanusers); }
	
	void 				setName(std::string name) { _name = name; }

	/*	API	*/
	void	userAdd(User& user);
	void	userRemove(User& user);
};

Channel& getChannel(std::list<Channel*>& channels, std::string chaname);

#endif