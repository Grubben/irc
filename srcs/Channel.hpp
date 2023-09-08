#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>

#include "Server.hpp"
#include "User.hpp"

class Channel
{
private:
    std::string _topic;
    Channel();

public:
    Channel(std::string name);
    ~Channel();

    const std::string name;
    // std::vector<User*> users;
};



#endif