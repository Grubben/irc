#ifndef SERVER_ENVIRONMENT_HPP
#define SERVER_ENVIRONMENT_HPP

#include <iostream>

class ServerEnvironment
{
private:
    int _portNumber;
    int _maxUsers;
    int _maxChannels;
    int _maxChannelUsers;
    int _maxChannelNameLength;
    int _maxNicknameLength;
    int _maxMessageLength;
    int _maxTopicLength;


public:
    ServerEnvironment();
    ServerEnvironment(int port);
    ~ServerEnvironment();
    int getPortNumber() const;
    int getMaxUsers() const;
    int getMaxChannels() const;
    int getMaxChannelUsers() const;
    int getMaxChannelNameLength() const;
    int getMaxNicknameLength() const;
    int getMaxMessageLength() const;
    int getMaxTopicLength() const;

};

#endif
