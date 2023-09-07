#include "User.hpp"

User::User(int userSocket)
{
    _socket = userSocket;
}

User::~User()
{
}

int User::getSocket() const
{
    return _socket;
}