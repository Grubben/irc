#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerEnvironment.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h> // using for perror , change this to c++ error handling
#include <unistd.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define BACKLOG 10

typedef struct sockaddr_in sockaddr_in;
/*
    To deal with struct sockaddr, programmers created
        a parallel structure: struct sockaddr_in (“in”
        for “Internet”) to be used with IPv4.

    And this is the important bit: a pointer to a struct 
        sockaddr_in can be cast to a pointer to a struct 
        sockaddr and vice-versa. So even though connect()
        wants a struct sockaddr*, you can still use a 
        struct sockaddr_in and cast it at the last minute!
    
    struct sockaddr_in {
        short int          sin_family;  // Address family, AF_INET
        unsigned short int sin_port;    // Port number
        struct in_addr     sin_addr;    // Internet address
        unsigned char      sin_zero[8]; // Same size as struct sockaddr
    };
*/

class Server
{
private:
    ServerEnvironment _environment;
    std::vector<User> _users;
    std::vector<Channel> _channels;
    
    // bircd method
    sockaddr_in _address;

    int _socket;

    Server();

public:
    Server(ServerEnvironment serverEnvironment);
    ~Server();
};

#endif