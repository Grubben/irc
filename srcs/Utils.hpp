#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <list>
#include "ServerMessage.hpp"
#include <algorithm>

int                         isDigit(char c);
std::string                 ltrim(std::string str);
std::string                 trim(std::string str);
std::string                 trim(std::string str, std::string undesired);
std::string                 removeCharacterFromString(std::string str, char c);
std::vector<std::string>    split(const std::string input, const std::string delimiter);
std::list<ServerMessage>    loadMessageIntoList(std::string message);

#endif
