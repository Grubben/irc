#ifndef SERVEREXCEPTIONS_HPP
#define SERVEREXCEPTIONS_HPP

#include <exception>

class SocketInitializationError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class SocketOptionsError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class SocketBindingError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class SocketListeningError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class SocketAcceptingError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class SocketSendingError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class SocketReceivingError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class ParserArgCountError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class ParserPortNumberError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class ParserPasswordTooBigError : public std::exception
{
    public:
        virtual const char* what() const throw();
};

class ParserNoPasswordError : public std::exception
{
    public:
        virtual const char* what() const throw();
};



#endif