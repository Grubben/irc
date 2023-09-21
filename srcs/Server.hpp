#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <fcntl.h>
#include <list>
#include <map>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <signal.h>
#include <sstream>

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

#include "Colors.hpp"

#include "Exceptions.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Utils.hpp"
#include "ServerMessage.hpp"
#include "NumericMacros.hpp"

extern bool g_isRunning;

#define BACKLOG 10
#define ACCEPTED_CHARS_NAME "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\`_^{|}-\0"
#define ACCEPTED_CHARS_PASS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()-_+={}[]|\\:;\"\'<>,.?/\0"


#define SERVER_HOSTNAME "localhost" // ou 127.0.0.1?
#define SERVER_NAME "ONossoIRC" // por mudar
#define OPERATOR_PASSWORD "1234"

// NOVO: retirar o que não é necessário e mudar valores
// #define MAX_USERS 10
// #define MAX_CHANNELS 10
// #define MAX_CHANNEL_USERS 10
// #define MAX_CHANNEL_NAME_LENGTH 10
// #define MAX_NICKNAME_LENGTH 10
// #define MAX_MESSAGE_LENGTH 10
// #define MAX_TOPIC_LENGTH 10


typedef struct addrinfo addrinfo;

class Server
{
private:
    std::map<int, User>             _users;
    std::map<std::string, Channel>  _channels;
    

	std::map<std::string, void (Server::*)(ServerMessage)> _commandMap;

    addrinfo                _address;
    addrinfo                _hints;
    socklen_t               _addr_size;
    fd_set                  _masterFDs;
    int                     _fdMax;
    int                     _listenSocket;
    
    std::string             _portNumber;
    std::string             _password;

    int                     _stop;

    void                    acceptConnection(int& fdMax);
    void                    dataReceived(int i);
    void                    channelCreate(std::string chaname); // Not public. if public will create empty channel
    void                    rpl_list(User& user);
public:
    Server(std::string port, std::string password);
    ~Server();
    

    void                    run();
    
    std::string             getPassword() const   { return (_password); };
    int                     getSocket() const     { return (_listenSocket); };
    User&	                getUserBySocket(int socket) {
        std::map<int, User>::iterator search = _users.find(socket);
        if (search != _users.end())
            return search->second;
        else
            throw SocketUnableToFindUser();
    };
    
    /*  API */
    void                    userCreate(int socket);
    void                    userQuit(const int sock);
    void                    userRmFromChannel(User& user, std::string chaname);
    // void                    channelDestroy(Channel& channel); //TODO: do we need this func?
    bool                    channelExists(std::string chaname);

    /* Commands*/
	void 	execute(std::list<ServerMessage> messageList);
    void    sendErrorMessage(int socket, std::string error);
    void    sendSuccessMessage(int socket, std::string numeric, std::string extraMessage);
	
    void	cap(ServerMessage serverMessage);
	void	pass(ServerMessage serverMessage);
	void	nick(ServerMessage serverMessage);
	void	user(ServerMessage serverMessage);
	void	oper(ServerMessage serverMessage);
	void	quit(ServerMessage serverMessage);
    void	privmsg(ServerMessage serverMessage);
    // Channel Operations
	void	join(ServerMessage serverMessage);
	void	part(ServerMessage serverMessage);
	void	topic(ServerMessage serverMessage);
    void    names(ServerMessage serverMessage);
    void    list(ServerMessage serverMessage);
    void    invite(ServerMessage serverMessage);
    void    kick(ServerMessage serverMessage);
    void    mode(ServerMessage serverMessage);
    void    who(ServerMessage serverMessage);

};

#endif
