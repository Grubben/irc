#include "Server.hpp"

void readFromClient(std::vector<std::string>::iterator it, int pos);

std::string removeCharacterFromString(std::string str, char toRemove)
{
    str.erase(std::remove(str.begin(), str.end(), toRemove), str.end());
    return str;
}

void messageHandler(std::string message)
{
    std::vector<std::string> messageBuffer = split(message, "\r\n");
    
    for (std::vector<std::string>::iterator it = messageBuffer.begin(); it != messageBuffer.end(); it++)
    {
        std::vector<std::string> messageParts = split(*it, " ");
        int i = 0;
        for (std::vector<std::string>::iterator it2 = messageParts.begin(); it2 != messageParts.end(); it2++)
        {
            readFromClient(it2, i);
            std::cout << "[" << i++ << "]" << *it2 << '-' << std::endl;
        }
    }
}

/*
    Section 4 of RFC 1459

    -- Where the reply ERR_NOSUCHSERVER is listed, it means that the
   <server> parameter could not be found.  The server must not send any
   other replies after this for that command.

*/

void readFromClient(std::vector<std::string>::iterator it, int pos)
{
    if (pos == 0) // command
    {
        
    }
}
