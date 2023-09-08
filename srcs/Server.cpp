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
    fd_set  masterFDs;
    fd_set  readFDs;
    std::string msg;

    FD_ZERO(&masterFDs);
    FD_ZERO(&readFDs);
    FD_SET(_socket, &masterFDs);
    int fdMax = _socket;


    while (true)
    {
        readFDs = masterFDs;

        if (select(fdMax + 1, &readFDs, NULL, NULL, NULL) == 0)
            continue;
        std::cout << "Poll triggered" << std::endl;

        if (isNewUser(readFDs))
        {
            if (validPassword(readFDs))
                acceptConnection(masterFDs, fdMax);
            else
                std::cout << "Invalid password" << std::endl;
        }

        msg = dataReceived(masterFDs, readFDs);

        // broadcast(msg);
    }
}

void    Server::broadcast(const std::string msg)
{
    std::vector<int>::iterator minit = clientFDs.begin();
    for (; minit != clientFDs.end(); minit++)
    {
        // if (*it != *minit)
        if (send(*minit, msg.c_str(), msg.length(), 0) == -1)
            throw SocketSendingError();
    }
}

std::string Server::dataReceived(fd_set &masterFDs, fd_set &readFDs)
{
    std::string msg;

    std::vector<int>::iterator it = clientFDs.begin();
    for (; it != clientFDs.end(); it++)
    {
        if (FD_ISSET(*it, &readFDs))
        {
            char message[1024] = "";
            int len = recv(*it, message, 1024, 0);
            if(len > 0)
            {
                message[len] = 0;

                std::cout << message << std::ends;
                msg = message;
            }
            else if (len == 0)
            {
                std::cout << "Client has left the network" << std::endl;
                close(*it);
                FD_CLR(*it, &masterFDs);
                //TODO: remove user from _users
            }
            else
                throw SocketReceivingError();
        }
    }
    return msg;
}

int Server::validPassword(fd_set& readFDs)
{
    // to be changed
    return (1);
}

void Server::acceptConnection(fd_set& masterFDs, int& fdMax)
{
    std::cout << "New connection" << std::endl;
    int newUserSocket = accept(_socket, (sockaddr *)&_address, (socklen_t*)&addr_size);
    if (newUserSocket == -1)
        throw SocketAcceptingError();
    fcntl(newUserSocket, F_SETFL, O_NONBLOCK);

    clientFDs.push_back(newUserSocket);

    //Adding new user to list
    _users.push_back(User(newUserSocket));

    FD_SET(newUserSocket, &masterFDs);
    fdMax = newUserSocket + 1;
}

int Server::isNewUser(fd_set& readFDs)
{
    return (FD_ISSET(_socket, &readFDs));
}

Server::Server()
{
}

Server::~Server()
{
    close(_socket);
    
}