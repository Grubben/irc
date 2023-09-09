#include "Server.hpp"

Server::Server(ServerEnvironment serverEnvironment)
{
    _environment = serverEnvironment;

    /* Socket initialization with protocol TCP */
    // listenSocket = socket(AF_INET, SOCK_STREAM, getprotobyname("TCP")->p_proto);
    listenSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1)
        throw SocketInitializationError();

    /* Set socket options to IPv4, port number and IP address. */
    _address.sin_family = AF_INET;
    _address.sin_port = htons(_environment.getPortNumber());
    _address.sin_addr.s_addr = INADDR_ANY; // inet_addr("some ip in here "); or assign INADDR_ANY to the s_addr field if you want to bind to your local IP address
    memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));

    addr_size = sizeof(_address); // Was Missing!!

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
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        throw SocketOptionsError();

    /* Bind socket to address */
    if (bind(listenSocket, (sockaddr *)&_address, addr_size) == -1)
        throw SocketBindingError();

    /* Listen on socket, Backlog is the number of connections allowed on the incoming queue */
    if (listen(listenSocket, BACKLOG) == -1)
        throw SocketListeningError();

    std::cout << "Server listening on port " << _environment.getPortNumber() << std::endl;

    /* Set socket to non-blocking */
    fcntl(listenSocket, F_SETFL, O_NONBLOCK);
}


void Server::run()
{
    fd_set  masterFDs;
    fd_set  readFDs;
    std::string msg;

    FD_ZERO(&masterFDs);
    FD_SET(listenSocket, &masterFDs);
    _fdMax = listenSocket;
    int n;

    while (true)
    {
        struct timeval tv = {200, 200};
        readFDs = masterFDs;

        n = select(_fdMax + 1, &readFDs, NULL, NULL, &tv);
        if (n == -1)
        {
            perror("select");
            exit(-1);
        }
        if (n == 0)
        {
            std::cout << "Timeout. Trying again..." << std::endl;
            continue;
        }
        std::cout << n << " polls triggered" << std::endl;


        if (FD_ISSET(listenSocket, &readFDs))
        {
            acceptConnection(masterFDs, _fdMax);
            if (n == 1)
                continue;
        }


        dataReceived(masterFDs, readFDs);
    }
}

void Server::acceptConnection(fd_set& masterFDs, int& fdMax)
{
    std::cout << "New connection" << std::endl;
    int newUserSocket = accept(listenSocket, (sockaddr *)&_address, &addr_size);

    if (newUserSocket == -1)
    {
        perror("acceptConnection");
        throw SocketAcceptingError();
    }
    fcntl(newUserSocket, F_SETFL, O_NONBLOCK);
    std::cout << "Creating new user with fd: " << newUserSocket << std::endl;

    FD_SET(newUserSocket, &masterFDs);

    if (newUserSocket > fdMax)
        fdMax = newUserSocket;

    users.push_back( new User(this, newUserSocket) );
}

std::string receiveMsg(int rcvFD)
{
    char message[1024] = "";
    int len = recv(rcvFD, message, sizeof(message) - 1, 0);

    if(len > 0)
    {
        message[len] = 0;
        return message;
    }
    else if (len == 0)
    {
        return "";
    }
    else
        throw SocketReceivingError();

}

void Server::dataReceived(fd_set &masterFDs, fd_set &readFDs)
{
    for (std::size_t i = listenSocket + 1; i <= _fdMax; i++)
    {
        if (FD_ISSET(i, &readFDs))
        {
            std::string msg = receiveMsg(i);
            if (msg == "")
            {
                std::cout << "Client has left the network. fd: " << i << std::endl;
                FD_CLR(i, &masterFDs);
                this->disconnect(i);
            }
            else
            {
                std::cout << msg;
            }
        }
    }
}

int Server::isNewUser(fd_set& readFDs)
{
    return (FD_ISSET(listenSocket, &readFDs));
}

Server::Server()
{
}

Server::~Server()
{
    close(listenSocket);
    
}


void    Server::disconnect(const int sock)
{
    std::cout << "Server is removing user with fd: " << sock << std::endl;
    std::cout << "users size: " << users.size() << std::endl;

    // std::vector<User*>::iterator it = users.begin();
    for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
    {
        if ((*it)->getSocket() == sock)
        {
            delete *it;
            users.erase(it);
            return;
        }
    }

}