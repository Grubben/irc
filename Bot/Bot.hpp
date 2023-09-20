#pragma once
#ifndef BOT_HPP
#define BOT_HPP

#define BOTNAME "iDUNNO"
#define CONNECTION_PASSWORD "adeus"
#define BOT_PORT 6697

#include "../srcs/Server.hpp"
#include "../srcs/Utils.hpp"

typedef struct hostent      hostent;
typedef struct sockaddr_in  sockaddr_in;

class Bot
{

private:
    int             _socket;
    std::string     _botNickname;
    std::string     _botUsername;


    hostent*        _host;
    sockaddr_in     _addr;


public:
    Bot();
    ~Bot() {close(_socket);};

    /*  Getters & Setters    */
    int             getSocket(void) const       { return (_socket); };
    std::string     getBotNickname(void) const  { return (_botNickname); };
    std::string     getBotUsername(void) const  { return (_botUsername); };

    const std::string     _summonWord;
};



#endif