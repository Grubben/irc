#include "Server.hpp"

Server::Server(int port, std::string password)
{
    _portNumber = port;
    _password = password;

    /* Socket initialization with protocol TCP */
    // listenSocket = socket(AF_INET, SOCK_STREAM, getprotobyname("TCP")->p_proto);
    _listenSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (_listenSocket == -1)
        throw SocketInitializationError();

    /* Set socket options to IPv4, port number and IP address. */
    _address.sin_family = AF_INET;
    _address.sin_port = htons(_portNumber);
    _address.sin_addr.s_addr = INADDR_ANY; // inet_addr("some ip in here "); or assign INADDR_ANY to the s_addr field if you want to bind to your local IP address
    memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));
    _addr_size = sizeof(_address);

    /* Set socket options to reuse address for fast server resets */
    int yes = 1;
    if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        throw SocketOptionsError();

    /* Bind socket to address */
    if (bind(_listenSocket, (sockaddr *)&_address, _addr_size) == -1)
        throw SocketBindingError();

    /* Listen on socket, Backlog is the number of connections allowed on the incoming queue */
    if (listen(_listenSocket, BACKLOG) == -1)
        throw SocketListeningError();

    std::cout << "Server listening on port " << _portNumber << std::endl;

    /* Set socket to non-blocking */
    fcntl(_listenSocket, F_SETFL, O_NONBLOCK);
}

Server::~Server()
{
    close(_listenSocket);

    //TODO: correct clean-up in case of weird stuff
    // _users.clear();  //Warning: Does not delete if there are any left
    // _channels.clear(); //Warning: Does not delete if there are any left
    //_
    
}

void Server::run()
{
    int n;
    fd_set  readFDs;
    std::string msg;

    FD_ZERO(&_masterFDs);
    FD_SET(_listenSocket, &_masterFDs);
    _fdMax = _listenSocket;

    for ( ;; )
    {
        struct timeval tv = {200, 200};
        readFDs = _masterFDs;

        n = select(_fdMax + 1, &readFDs, NULL, NULL, &tv);
        if (n == -1)
        {
            perror("select");
            throw SocketSelectingError();
        }
        else if (n == 0)
        {
            std::cout << "Timeout. Trying again..." << std::endl;
            continue;
        }
        std::cout << n << " polls triggered" << std::endl;

        for (int i = 0; i <= _fdMax; i++)
        {
            if (FD_ISSET(i, &readFDs))
            {
                if (i == _listenSocket)
                    acceptConnection(_fdMax);
                else
                    dataReceived(i);
            }
        }
    }
}

void Server::acceptConnection(int& fdMax)
{
    std::cout << "New connection" << std::endl;

    sockaddr_in newUserAddress;
    socklen_t newUserAddrSize = sizeof(newUserAddress);

    int newUserSocket = accept(_listenSocket, (sockaddr *)&newUserAddress, &newUserAddrSize);
    if (newUserSocket == -1)
    {
        perror("accept:");
        throw SocketAcceptingError();
    }
    std::cout << "Creating new user with fd: " << newUserSocket << std::endl;
    fcntl(newUserSocket, F_SETFL, O_NONBLOCK);
    FD_SET(newUserSocket, &_masterFDs);
    if (newUserSocket > fdMax)
        fdMax = newUserSocket;
    userCreate(newUserSocket);
    userAddToChannel(getUserBySocket(newUserSocket), "#hello");
}

void Server::dataReceived(int i)
{
    char message[1024] = "";
    int len;;
    if ((len = recv(i, message, sizeof(message) - 1, 0) <= 0))
    {
        if (len == 0)
            std::cout << "Client has left the network. fd: " << i << std::endl;
        else
        {
            perror("recv");
        }
        message[len] = 0;
        close(i);
        FD_CLR(i, &_masterFDs);
        this->userQuit(i);
    }
    else
    {
        getUserBySocket(i).says(message, this);
        std::cout << message;

        //send message to all clients but ourself and listenSocket
        //for (int j = _listenSocket + 1; j <= _fdMax; j++)
        //{
        //    if (FD_ISSET(j, &_masterFDs))
        //    {
        //        if (j != _listenSocket && j != i)
        //        {
        //            if (send(j, message, len, 0) == -1) //maybe need to make the sendall() function
        //                perror("send");
        //        }
        //    }
        //}
    }

}

void    Server::userQuit(const int sock)
{
    std::cout << "Server is removing user with fd: " << sock << std::endl;

    User*   userdelete = &getUserBySocket(sock);

    _users.remove(userdelete);

    delete userdelete;

    std::cout << "users size: " << _users.size() << std::endl;
}

void    Server::userCreate(int socket)
{
    _users.push_back( new User(this, socket) );
}


void    Server::userAddToChannel(User& user, std::string chaname)
{
    Channel*    chan;
    try
    {
        chan = &getChannel(_channels, chaname);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        chan = channelCreate(chaname);
    }

    user.channelSubscribe(chan);
    chan->userAdd(user);
}

void    Server::userRmFromChannel(User& user, Channel& channel)
{
    channel.userRemove(user);
}

Channel*    Server::channelCreate(std::string chaname)
{
    Channel*    chan = new Channel(this, chaname);
    _channels.push_back( chan );
    return chan;
}

void    Server::channelDestroy(Channel& channel)
{
    channel.usersDrop();
    _channels.remove(&channel);
    delete &channel;
}