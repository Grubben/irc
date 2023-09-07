#include "ServerEnvironment.hpp"

ServerEnvironment::ServerEnvironment(int port)
{
    _portNumber = port;
    
    // Values to be changed later
    _maxUsers = 4;
    _maxChannels = 4;
    _maxChannelUsers = 4;
    _maxChannelNameLength = 32;
    _maxNicknameLength = 32;
    _maxMessageLength = 256;
    _maxTopicLength = 1024;
}

ServerEnvironment::ServerEnvironment()
{
}

ServerEnvironment::~ServerEnvironment()
{
}

int ServerEnvironment::getPortNumber() const
{
    return _portNumber;
}

int ServerEnvironment::getMaxUsers() const
{
    return _maxUsers;
}

int ServerEnvironment::getMaxChannels() const
{
    return _maxChannels;
}

int ServerEnvironment::getMaxChannelUsers() const
{
    return _maxChannelUsers;
}

int ServerEnvironment::getMaxChannelNameLength() const
{
    return _maxChannelNameLength;
}

int ServerEnvironment::getMaxNicknameLength() const
{
    return _maxNicknameLength;
}

int ServerEnvironment::getMaxMessageLength() const
{
    return _maxMessageLength;
}

int ServerEnvironment::getMaxTopicLength() const
{
    return _maxTopicLength;
}

