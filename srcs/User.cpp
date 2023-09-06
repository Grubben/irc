#include "User.hpp"

User::User(std::string name)
{
    _name = name;
    _state = "active";
}

User::~User()
{
}
