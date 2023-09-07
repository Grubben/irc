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
    struct timeval tv;

    fd_set  master;
    fd_set  readfds;

    tv.tv_sec = 200;
    tv.tv_usec = 500000;

    std::vector<int> clientfds;

    FD_ZERO(&master);
    FD_ZERO(&readfds);


    FD_SET(_socket, &master);

    int fdmax = _socket;

    while (true)
    {
        readfds = master;

        if (select(fdmax + 1, &readfds, NULL, NULL, &tv) == 0)
            continue;
        std::cout << "Poll triggered" << std::endl;

        if (FD_ISSET(_socket, &readfds)) // New user
        {
            std::cout << "New connection" << std::endl;
            int newuser_socket = accept(_socket, (sockaddr *)&_address, (socklen_t*)&addr_size);
            fcntl(newuser_socket, F_SETFL, O_NONBLOCK);

            clientfds.push_back(newuser_socket);

            //Adding new user to list
            _users.push_back(User(newuser_socket));

            FD_SET(newuser_socket, &master);
            fdmax = newuser_socket + 1;
        }

        std::vector<int>::iterator it = clientfds.begin();

        for (; it != clientfds.end(); it++)
        {
            if (FD_ISSET(*it, &readfds))
            {
                char message[1024] = "";
                int len;
                if((len = recv(*it, message, 1024, 0)) > 0)
                {
                    message[len] = 0;
                    std::cout << message;
                }
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