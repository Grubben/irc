#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include "ServerMessage.hpp"

int                         isDigit(char c);
std::string                 ltrim(std::string str);
std::string                 trim(std::string str);
std::string                 trim(std::string str, std::string undesired);
std::string                 removeCharacterFromString(std::string str, char c);
std::vector<std::string>    split(const std::string input, const std::string delimiter);

template <typename T>
std::string cont2str(T& svec)
{
    std::string string = "";
    // for (size_t i = 0; i < svec.size(); i++)
    for (typename T::iterator it = svec.begin(); it != svec.end(); it++)
        string += " " + *svec.find(*it);
    string.erase(0,1); // erase first space
    return string;
}

int                     sendAll(int socket, std::string msg);
#endif
