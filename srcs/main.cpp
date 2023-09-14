#include "Server.hpp"

void parseInput(int argc, char **argv);
void checkPortNumber(char *port);
void checkPassword(std::string password);

bool g_isRunning = true;

void    sighandler(int signum)
{
    (void) signum;
    g_isRunning = false;
}

int main(int argc, char **argv)
{
    std::cout << std::unitbuf;
    
    try
    {
        parseInput(argc, argv);
        Server server(argv[1], argv[2]);        
        
        signal(SIGINT, sighandler);

        server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    return 0;
}

void checkPortNumber(char *port)
{
    for (int i = 0; port[i]; i++)
    {
        if (!isDigit(port[i]))
            throw ParserPortNumberError();
    }

    if (atoi(port) < 0 || atoi(port) > 65535)
        throw ParserPortNumberError();
}

void checkPassword(std::string password)
{
    if (password.length() > 32)
        throw ParserPasswordTooBigError();
    if (password.empty())
		throw ParserNoPasswordError();
}

void parseInput(int argc, char **argv)
{
    // change this in the end to only work with password in , argc != 3
    if (argc > 3 || argc < 2)
        throw ParserArgCountError();

    checkPortNumber(argv[1]);
    if (argc == 3)
        checkPassword(argv[2]);
    else if (argc == 2)
        argv[2] = (char *)"";
}
