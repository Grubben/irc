#include "Server.hpp"
#include "Exceptions.hpp"

const char* SocketInitializationError::what() const throw()
{
    return "Socket initialization error";
}

const char* SocketOptionsError::what() const throw()
{
    return "Socket options error";
}

const char* SocketBindingError::what() const throw()
{
    return "Socket binding error";
}

const char* SocketListeningError::what() const throw()
{
    return "Socket listening error";
}

const char* SocketAcceptingError::what() const throw()
{
    return "Socket accepting error";
}

const char* SocketSendingError::what() const throw()
{
    return "Socket sending error";
}

const char* ParserArgCountError::what() const throw()
{
    return "Usage: ./ircserv [port]";
}

const char* ParserPortNumberError::what() const throw()
{
    return "Port must be between 0 and 65535";
}