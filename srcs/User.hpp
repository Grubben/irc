#ifndef USER_HPP
#define USER_HPP

#pragma once
#include <iostream>
#include <unistd.h>
#include <vector>
#include <list>
#include <map>
#include "ServerMessage.hpp"
#include "Utils.hpp"

class Server;
class Channel;

class User
{
private:
	Server&				_server;
	const int			_socket;
	std::map<std::string, Channel*> _userChannels;
	
	std::string			_username;
	std::string			_nickname;

	std::string			_buffer;

	bool 				_isLoggedIn; // if false, user cannot access commands apart from PASS, NICK, USER, OPER(?), QUIT
	bool 				_isOperator;

	void	channelsDrop(); //Not API: only used internally

public:
	User(Server& server, int userSocket);
	User(const User& copy);
	User& operator= (const User& copy);
	virtual ~User(void);

    bool    			operator==(const int sock) 	{ return this->_socket == sock; };
	
	const int& 			getSocket() const 			{ return _socket; }
	const std::string&	getNick() const 			{ return _nickname; }
	const std::string&	getName() const 			{ return _username; }
	const std::string&	getBuffer() const 			{ return _buffer; }

	std::string			setbuffer(std::string toChange) {return toChange;};
	
	void	addBuffer(std::string msg)
	{
		_buffer += msg; 
	}

	std::string	flushBuffer(void)
	{
		std::string ret = _buffer;
		_buffer = "";
		return ret;
	}


	void	setNick(std::string nickname) 	{ this->_nickname = nickname; }
	void	setName(std::string username) 	{ this->_username = username; }

	/* API */
    void    says(std::string message);
	
	void	channelJoin(Channel& channel);
	void	channelPart(std::string chaname);
	//TODO:	serverQuit

};

#endif