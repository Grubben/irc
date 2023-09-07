#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "User.hpp"
#include <vector>

class Channel
{
private:
    std::string _name;
    std::string _topic;
    std::vector<User> _users;
    Channel();

public:
    Channel(std::string name);
    ~Channel();
};



#endif