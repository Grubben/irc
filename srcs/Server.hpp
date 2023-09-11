#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerEnvironment.hpp"
#include "Exceptions.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "InputIRCParser.hpp"

#include <fcntl.h>
#include <list>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>


#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define BACKLOG 10

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
typedef struct sockaddr_in sockaddr_in;

class Server
{
private:
    ServerEnvironment _environment;

    fd_set  _masterFDs;
    int _fdMax;
    std::vector<User*> _users;

    sockaddr_in _address;
    socklen_t addr_size;
    int listenSocket;

    Server();
    int isNewUser(fd_set& readFDs);
    bool validPassword(fd_set& readFDs);
    void acceptConnection(int& fdMax);
    void dataReceived(fd_set& readFDs);


public:
    Server(ServerEnvironment serverEnvironment);
    ~Server();

    void    run();

    void    disconnect(int sock);

    void    broadcast(const std::string msg);
    

    // std::list<int>  clientFDs;
    // static std::vector<Channel> _channels;
};

#endif
