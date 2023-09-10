#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerEnvironment.hpp"
#include "Exceptions.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "stringFuncs.hpp"
#include "ServerMessage.hpp"

#include <fcntl.h>
#include <list>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <vector>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 10

#define SERVER_HOSTNAME "localhost" // ou 127.0.0.1?
#define SERVER_NAME "ONossoIRC" // por mudar

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
    std::list<User> _users;
    std::vector<Channel> _channels;

    std::vector<int> _clientFDs;

    sockaddr_in _address;
    socklen_t addr_size;
    int _socket;

    Server();
    int isNewUser(fd_set& readFDs);
    void acceptConnection(fd_set& masterFDs, int& fdMax);
    void dataReceived(fd_set& masterFDs, fd_set& readFDs);
    void messageHandler(std::string message);


public:
    Server(ServerEnvironment serverEnvironment);
    ~Server();

    void    passwordVerification(std::string password);
    void    run();
    void    broadcast(const std::string msg);
    
    ServerEnvironment getEnvironment() const;
    std::vector<int> getClientFDs() const;
};

void sendNumericResponse(int clientSocket, int numericCode, const std::string& message);

#endif
