#include "Server.hpp"

Server::Server(ServerEnvironment serverEnvironment)
{
    _environment = serverEnvironment;

    /* Socket initialization with protocol TCP */
    _socket = socket(AF_INET, SOCK_STREAM, getprotobyname("TCP")->p_proto);
    if (_socket == -1)
        throw SocketInitializationError();

    /* Set socket options to IPv4, port number and IP address. */
    _address.sin_family = AF_INET;
    _address.sin_port = htons(_environment.getPortNumber());
    _address.sin_addr.s_addr = INADDR_ANY; // inet_addr("some ip in here "); or assign INADDR_ANY to the s_addr field if you want to bind to your local IP address
    memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));

    /*
        When a network socket is closed, it normally enters the TIME_WAIT
            state for a certain period of time. This is basically the 
            operating system reserving the socket address to ensure that
            any delayed or out-of-order packets are not mistakenly associated
            with a new connection.

        By setting the SO_REUSEADDR, we can reuse the same socket address
            immediately after it has been closed (even if it is in TIME_WAIT
            state).
    */
    int yes = 1;
    if (setsockopt(_socket,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        throw SocketOptionsError();

    /* Bind socket to address */
    if (bind(_socket, (sockaddr *)&_address, sizeof(_address)) == -1)
        throw SocketBindingError();

    /* Listen on socket, Backlog is the number of connections allowed on the incoming queue */
    if (listen(_socket, BACKLOG) == -1)
        throw SocketListeningError();

    std::cout << "Server listening on port " << _environment.getPortNumber() << std::endl;

    /* Set socket to non-blocking */
    fcntl(_socket, F_SETFL, O_NONBLOCK);
}


void Server::run()
{
    while (true)
    {
        
        int new_socket = accept(_socket, (sockaddr *)&_address, (socklen_t*)&addr_size);
        if (new_socket != -1)
        {
            fcntl(new_socket, F_SETFL, O_NONBLOCK);

            _users.push_back(User(new_socket));
            
            std::cout << "New connection from " << inet_ntoa(_address.sin_addr) << ":" << ntohs(_address.sin_port) << std::endl;
        }

        std::list<User>::iterator it = _users.begin();
        for (; it != _users.end(); it++)
        {
            char message[1024];
            int len;
            if((len = recv(it->getSocket(), message, 1024, 0)) > 0)
            {
                message[len] = 0;
                std::cout << message;
            }
        }
               
    }
}


Server::Server()
{
}

Server::~Server()
{
    close(_socket);
}