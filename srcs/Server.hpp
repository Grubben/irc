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
#include <stdio.h>
#include <errno.h>

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
    ServerEnvironment       _environment;
    std::list<User*>        _users;
    std::list<Channel*>    _channels;

    fd_set                  _masterFDs;

    int                     _fdMax;

    sockaddr_in             _address;
    socklen_t               _addr_size;
    int                     _listenSocket;

    int                     isNewUser(fd_set& readFDs);
    void                    acceptConnection(int& fdMax);
    void                    dataReceived(fd_set& readFDs);
    void                    broadcast(const std::string msg);


    Channel*                channelCreate(std::string chaname); // Not public. if public will create empty channel
public:
    Server(ServerEnvironment serverEnvironment);
    ~Server();

    void                    run();
    
    ServerEnvironment       getEnvironment() const;
    std::vector<int>        getClientFDs() const;
    std::list<User*>        getUsers() const;
    sockaddr_in             getAddress() const;
    socklen_t               getAddrSize() const;
    int                     getSocket() const;

    User&	                getUserBySocket(int socket);

    void                    setEnvironment(ServerEnvironment environment);
    void                    setClientFDs(std::vector<int> clientFDs);
    void                    setAddress(sockaddr_in address);
    void                    setAddrSize(socklen_t addrSize);
    void                    setSocket(int socket);

    /*  API */
    void                    userCreate(int socket);
    void                    userDisconnect(const int sock);

    void                    userAddToChannel(User& user, std::string chaname);
    void                    userRmFromChannel(User& user, Channel& channel);

    void                    channelDestroy(Channel& channel);

};

void sendNumericResponse(int clientSocket, int numericCode, const std::string& message);

#endif
