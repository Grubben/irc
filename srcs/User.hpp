#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>


// There will be a user linked list to store all the clients connected to the server
class User
{
private:
    int     _socket;
    User();

public:
    User(int userSocket);
    ~User();

    int getSocket() const;
};

#endif
