#pragma once
#include <iostream>
#include <unistd.h>
#include <vector>
#include <list>
#include "ServerMessage.hpp"
#include "stringFuncs.hpp"

class Server;
class Channel;

class User
{
private:
	Server *_server;
	int		_socket;

	std::list<Channel*>	_channels;

	// User();

	void messageHandler(std::string message, Server *server);
	int capabilityNegotiation();
	int connectionRegistration(std::list<ServerMessage> messageList, Server* server);
	int miscellaneousMessages(std::list<ServerMessage> messageList, Server *server);
	int userBasedQueries(std::list<ServerMessage> messageList, Server *server);
	int sendingMessages(std::list<ServerMessage> messageList, Server *server);
	int channelOperations(std::list<ServerMessage> messageList, Server *server);
	
public:
	User(Server* server, int userSocket);
	User(const User& copy);
	virtual ~User(void);

	User&	operator= (const User& copy);

    bool    operator==(const int sock);

	int getSocket() const;

    void    quitServer();



	std::string	name;
	std::string	nick;

	/* API */
    void    says(std::string message, Server *server);
	
	void	channelJoin(Server* server, std::string chaname);
	// void	channelSubscribe(Server* server, Channel& channel);
 
	void	channelLeave(Server* server, Channel& channel);

	void	channelsDrop();

};
