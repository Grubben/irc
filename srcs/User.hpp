#pragma once
#include <iostream>
#include <unistd.h>
#include <vector>
#include "stringFuncs.hpp"

class Server;

class User
{
private:
	Server*	_server;
	int		sock;
	User();

public:
	User(Server* server, int userSocket);
	User(const User& copy);
	virtual ~User(void);

	User&	operator= (const User& copy);

    bool    operator==(const int sock);

	int getSocket() const;

    void    quitServer();

    void    says(std::string message);

	std::string	name;
	std::string	nick;
};
