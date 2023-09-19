#include "Utils.hpp"

std::string ltrim(std::string str)
{
    std::size_t found = str.find_first_not_of(" \t\r\n");

    if (found == std::string::npos)
        return str;

    str = str.substr(found);

    return str;
}

std::string trim(std::string str)
{
    str = ltrim(str);

    std::size_t found = str.find_last_not_of(" \t\r\n");
    str = str.substr(0, found + 1);

    return str;
}

std::string trim(std::string str, std::string undesired)
{
    std::size_t found = str.find_first_not_of(undesired);

    str = str.substr(found);

    found = str.find_last_not_of(undesired);
    str = str.substr(0, found + 1);

    return str;
}

std::vector<std::string> split(const std::string input, const std::string delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = input.find(delimiter);

    while (end != std::string::npos)
    {
        if (input.substr(start, end - start) != "")
        {
            result.push_back(input.substr(start, end - start));
        }
        start = end + delimiter.length();
        while (start < input.length() && input[start] == ' ')
        {
            start++;
        }

        end = input.find(delimiter, start);
    }
    if (start < input.length())
    {
        result.push_back(input.substr(start));
    }
    return result;
}

std::string removeCharacterFromString(std::string str, char toRemove)
{
    str.erase(std::remove(str.begin(), str.end(), toRemove), str.end());
    return str;
}

int isDigit(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    return 0;
}

int sendAll(int socket, std::string msg)
{
    long long n = 0;

    while (n < (long long)msg.size())
    {
        n += send(socket, msg.c_str() + n, msg.size() - n, 0);
        if (n == -1)
            return -1;
    }
    return 0;
}