#include "Server.hpp"

Server::Server(ServerEnvironment serverEnvironment)
{
    _environment = serverEnvironment;
    _socket = socket(AF_INET, SOCK_STREAM, getprotobyname("TCP")->p_proto);
    if (_socket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }
    _address.sin_family = AF_INET; // IPv4
    _address.sin_addr.s_addr = INADDR_ANY; // inet_addr("some ip in here "); or assign INADDR_ANY to the s_addr field if you want to bind to your local IP address
    _address.sin_port = htons(_environment.getPortNumber()); // sin_port must be in network byte order
    memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));
    
    // Supostamente isto e para quando o bind() falha e o port esta em uso, nao sei se e preciso ou se realmente funciona porque o erro e o mesmo. (acontece quando o user da o input entre 0 e 1024)
    int yes = 1;
    if (setsockopt(_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    } 

    if (bind(_socket, (sockaddr *)&_address, sizeof(_address)) == -1)
    {
        std::cerr << "Error binding socket" << std::endl;
        exit(1);
    }
    if (listen(_socket, BACKLOG) == -1)
    {
        std::cerr << "Error listening on socket" << std::endl;
        exit(1);
    }
    std::cout << "Server listening on port " << _environment.getPortNumber() << std::endl;

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int newSocket = accept(_socket, (sockaddr *)&their_addr, (socklen_t*)&addr_size);


    char msg[512] = "Hello World !\n";
    int len, bytes_sent;
    len = strlen(msg);
    bytes_sent = send(newSocket, msg, len, 0);


    close(newSocket);
    close(_socket);
}

Server::Server()
{
}

Server::~Server()
{
}