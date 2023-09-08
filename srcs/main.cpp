#include "Server.hpp"

void parseInput(int argc, char **argv);
void checkPortNumber(char *port);
void checkPassword(char *password);

int main(int argc, char **argv)
{
    try
    {
        parseInput(argc, argv);

        // init server environment
        ServerEnvironment serverEnvironment(atoi(argv[1]), argv[2]);

        // start server
        Server server(serverEnvironment);

        server.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    
    // start server loop
    

    return 0;
}

void checkPortNumber(char *port)
{
    for (int i = 0; port[i]; i++)
    {
        if (!isdigit(port[i]))
            throw ParserPortNumberError();
    }

    if (atoi(port) < 0 || atoi(port) > 65535)
        throw ParserPortNumberError();
}

void checkPassword(char *password)
{
    if (strlen(password) > 32)
        throw ParserPasswordError();
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
