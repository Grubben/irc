#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>


// There will be a user linked list to store all the clients connected to the server
class User
{
private:
    std::string _name;
    std::string _state;
    User();

public:
    User(std::string name);
    ~User();
};

#endif
