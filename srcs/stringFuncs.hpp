#include <iostream>
#include <vector>
#include <algorithm>

int isDigit(char c);
std::string ltrim(std::string str);
std::string trim(std::string str);
std::string trim(std::string str, std::string undesired);
std::vector<std::string> split(const std::string input, const std::string delimiter);
std::string removeCharacterFromString(std::string str, char c);