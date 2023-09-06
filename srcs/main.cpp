#include "Server.hpp"

void parseInput(int argc, char **argv);

int main(int argc, char **argv)
{
    parseInput(argc, argv);

    // init server environment
    ServerEnvironment serverEnvironment(atoi(argv[1]));

    // start server
    Server server(serverEnvironment);


    
    


    return 0;
}

void parseInput(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./ircserv [port]" << std::endl;
        exit(1);
    }
    for (int i = 0; argv[1][i]; i++)
    {
        if (!isdigit(argv[1][i]))
        {
            std::cerr << "Port must be a number" << std::endl;
            exit(1);
        }
    }
    if (atoi(argv[1]) < 0 || atoi(argv[1]) > 65535)
    {
        std::cerr << "Port must be between 0 and 65535" << std::endl;
        exit(1);
    }
}
