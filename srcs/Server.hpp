#ifndef SERVER_HPP
#define SERVER_HPP

#include "Exceptions.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Utils.hpp"
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

// NOVO: retirar o que não é necessário e mudar valores
// #define MAX_USERS 10
// #define MAX_CHANNELS 10
// #define MAX_CHANNEL_USERS 10
// #define MAX_CHANNEL_NAME_LENGTH 10
// #define MAX_NICKNAME_LENGTH 10
// #define MAX_MESSAGE_LENGTH 10
// #define MAX_TOPIC_LENGTH 10

typedef struct sockaddr_in sockaddr_in;

class Server
{
private:
    // std::list<User*>        _users;
    std::map<int, User>             _users;
    // std::list<Channel*>     _channels;
    std::map<std::string, Channel>  _channels;
    
    sockaddr_in             _address;
    socklen_t               _addr_size;
    fd_set                  _masterFDs;
    int                     _fdMax;
    int                     _listenSocket;
    int                     _portNumber;
    std::string             _password;

    void                    acceptConnection(int& fdMax);
    void                    dataReceived(int i);

    void                channelCreate(std::string chaname); // Not public. if public will create empty channel

public:
    Server(int port, std::string password);
    ~Server();

    void                    run();
    
    std::string             getPassword() const   { return (_password); };
    int                     getPortNumber() const { return (_portNumber); };
    // std::list<User*>        getUsers() const      { return (_users); };
    // std::list<Channel*>     getChannels() const   { return (_channels); };
    sockaddr_in             getAddress() const    { return (_address); };
    socklen_t               getAddrSize() const   { return (_addr_size); };
    int                     getSocket() const     { return (_listenSocket); };
    User&	                getUserBySocket(int socket) {
        std::map<int, User>::iterator search = _users.find(socket);
        if (search != _users.end())
            return search->second;
        else
            throw SocketUnableToFindUser();
    };

    void                    setAddress(sockaddr_in address) { _address = address; };
    void                    setAddrSize(socklen_t addrSize) { _addr_size = addrSize; };
    void                    setSocket(int socket)           { _listenSocket = socket; };    

    /*  API */
    void                    userCreate(int socket);
    void                    userQuit(const int sock);
    void                    userAddToChannel(User& user, std::string chaname); // Creates channel if non-existant
    void                    userRmFromChannel(User& user, std::string chaname);
    // void                    channelDestroy(Channel& channel); //TODO: do we need this func?
};

void sendNumericResponse(int clientSocket, int numericCode, const std::string& message);

#endif
