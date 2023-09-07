#include "Server.hpp"

void parseInput(int argc, char **argv);

int main(int argc, char **argv)
{
    try
    {
        parseInput(argc, argv);

        // init server environment
        ServerEnvironment serverEnvironment(atoi(argv[1]));

        // start server
        Server server(serverEnvironment);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    
    // start server loop
    //server.run();

    return 0;
}

void parseInput(int argc, char **argv)
{
    // Vai ser preciso melhorar o parser do input tendo em conta a password
    if (argc != 2)
        throw ParserArgCountError();
    for (int i = 0; argv[1][i]; i++)
    {
        if (!isdigit(argv[1][i]))
        {
            std::cerr << "Port must be a number" << std::endl;
            exit(1);
        }
    }
    if (atoi(argv[1]) < 0 || atoi(argv[1]) > 65535)
        throw ParserPortNumberError();
}
