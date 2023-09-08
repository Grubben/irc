#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>
#include <vector>
#include "stringFuncs.hpp"

class Server;

// There will be a user linked list to store all the clients connected to the server
class User
{
private:
    Server*  _server;
    int     _socket;
    User();

public:
    User(Server* server, int userSocket);
    ~User();

    int getSocket() const;

    void    quitServer();

    void    says(std::string message);

    bool    operator==(const User user);
};

#endif
