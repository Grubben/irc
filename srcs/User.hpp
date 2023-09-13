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
	Server*				_server;
	const int			_socket;
	std::list<Channel*>	_channels;
	std::map<std::string, void (User::*)(std::list<ServerMessage>, Server*)> _commandMap;
	
	std::string			_username;
	std::string			_nickname;

	bool 				_isLoggedIn; // if false, user cannot access commands apart from PASS, NICK, USER, OPER(?), QUIT
	bool 				_isOperator;

	// User();

public:
	User(Server* server, int userSocket);
	User(const User& copy);
	User& operator= (const User& copy);
	virtual ~User(void);

    bool    	operator==(const int sock) 	{ return this->_socket == sock; };
	
	const int& 	getSocket() const 			{ return _socket; }
	std::string getNick() const 			{ return _nickname; }
	std::string getName() const 			{ return _username; }

	
	void 		setNick(std::string nickname) 	{ this->_nickname = nickname; }
	void 		setName(std::string username) 	{ this->_username = username; }

	/* API */
    void    says(std::string message, Server *server);
	
	void	channelJoin(Server* server, std::string chaname);
	void	channelSubscribe(Channel* channel); // Only called by server
	void	channelPart(Server* server, Channel& channel);
	void	channelsDrop();


	/* Commands*/
	void 	execute(std::list<ServerMessage> messageList, Server* server);
	void	cap(std::list<ServerMessage> messageList, Server* server);
	void	pass(std::list<ServerMessage> messageList, Server* server);
	void	nick(std::list<ServerMessage> messageList, Server* server);
	void	user(std::list<ServerMessage> messageList, Server* server);
	void	oper(std::list<ServerMessage> messageList, Server* server);
	void	quit(std::list<ServerMessage> messageList, Server* server);
	void	join(std::list<ServerMessage> messageList, Server* server);
	void	part(std::list<ServerMessage> messageList, Server* server);
	void	mode(std::list<ServerMessage> messageList, Server* server);
	void	topic(std::list<ServerMessage> messageList, Server* server);


};

#endif