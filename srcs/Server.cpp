#include "Server.hpp"

Server::Server(ServerEnvironment serverEnvironment)
{
    _environment = serverEnvironment;

    /* Socket initialization with protocol TCP */
    // listenSocket = socket(AF_INET, SOCK_STREAM, getprotobyname("TCP")->p_proto);
    _listenSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (_listenSocket == -1)
        throw SocketInitializationError();

    /* Set socket options to IPv4, port number and IP address. */
    _address.sin_family = AF_INET;
    _address.sin_port = htons(_environment.getPortNumber());
    _address.sin_addr.s_addr = INADDR_ANY; // inet_addr("some ip in here "); or assign INADDR_ANY to the s_addr field if you want to bind to your local IP address
    memset(_address.sin_zero, '\0', sizeof(_address.sin_zero));

    _addr_size = sizeof(_address); // Was Missing!!

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
    if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        throw SocketOptionsError();

    /* Bind socket to address */
    if (bind(_listenSocket, (sockaddr *)&_address, _addr_size) == -1)
        throw SocketBindingError();

    /* Listen on socket, Backlog is the number of connections allowed on the incoming queue */
    if (listen(_listenSocket, BACKLOG) == -1)
        throw SocketListeningError();

    std::cout << "Server listening on port " << _environment.getPortNumber() << std::endl;

    /* Set socket to non-blocking */
    fcntl(_listenSocket, F_SETFL, O_NONBLOCK);
}


void Server::run()
{
    fd_set  readFDs;
    std::string msg;

    FD_ZERO(&_masterFDs);
    FD_SET(_listenSocket, &_masterFDs);
    _fdMax = _listenSocket;
    int n;

    while (true)
    {
        struct timeval tv = {200, 200};
        readFDs = _masterFDs;

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


        if (FD_ISSET(_listenSocket, &readFDs))
        {
            acceptConnection(_fdMax);
            // if (n == 1)
            //     continue;
        }
        dataReceived(readFDs);
    }
}

void Server::acceptConnection(int& fdMax)
{
    std::cout << "New connection" << std::endl;
    int newUserSocket = accept(_listenSocket, (sockaddr *)&_address, &_addr_size);

    if (newUserSocket == -1)
    {
        perror("acceptConnection");
        throw SocketAcceptingError();
    }
    fcntl(newUserSocket, F_SETFL, O_NONBLOCK);
    std::cout << "Creating new user with fd: " << newUserSocket << std::endl;

    FD_SET(newUserSocket, &_masterFDs);

    if (newUserSocket > fdMax)
        fdMax = newUserSocket;

    _users.push_back( new User(this, newUserSocket) );

    _users.back()->channelJoin(this, "#hello");
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

void Server::dataReceived(fd_set &readFDs)
{
    for (std::size_t i = _listenSocket + 1; i <= _fdMax; i++)
    {
        if (FD_ISSET(i, &readFDs))
        {
            std::string msg = receiveMsg(i);
            if (msg == "")
            {
                std::cout << "Client has left the network. fd: " << i << std::endl;
                FD_CLR(i, &_masterFDs);
                this->disconnect(i);
            }
            else
            {
                //TODO: meter aqui o message Handler com o .says()
                getUserBySocket(i).says(msg, this);
                std::cout << msg;
            }
        }
    }
}

int Server::isNewUser(fd_set& readFDs)
{
    return (FD_ISSET(_listenSocket, &readFDs));
}



std::string ServerMessage::getPrefix() const
{
    return _prefix;
}

std::string ServerMessage::getCommand() const
{
    return _command;
}

std::vector<std::string> ServerMessage::getParams() const
{
    return _params;
}

Server::Server()
{
}

Server::~Server()
{
    close(_listenSocket);

    //TODO: correct clean-up in case of weird stuff
    _users.clear();  //Warning: Does not delete if there are any left
    _channels.clear(); //Warning: Does not delete if there are any left
    //_
    
}

User&	Server::getUserBySocket(int socket)
{
    for (std::list<User*>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        if ((*it)->getSocket() == socket)
        {
            return *(*it);
        }
    }
    throw SocketUnableToFindUser();
}


void    Server::disconnect(const int sock)
{
    std::cout << "Server is removing user with fd: " << sock << std::endl;

    User*   userdelete = &getUserBySocket(sock);

    userdelete->channelsDrop();

    _users.remove(userdelete);

    delete userdelete;

    std::cout << "users size: " << _users.size() << std::endl;
}
ServerEnvironment Server::getEnvironment() const
{
    return _environment;
}

std::list<User*> Server::getUsers() const
{
    return _users;
}

// std::vector<Channel> Server::getChannels() const
// {
//     return _channels;
// }

sockaddr_in Server::getAddress() const
{
    return _address;
}

socklen_t Server::getAddrSize() const
{
    return _addr_size;
}

int Server::getSocket() const
{
    return _listenSocket;
}

void Server::setEnvironment(ServerEnvironment environment)
{
    _environment = environment;
}

// void Server::setUsers(std::list<User> users)
// {
//     _users = users;
// }

// void Server::setChannels(std::vector<Channel> channels)
// {
//     _channels = channels;
// }

void Server::setAddress(sockaddr_in address)
{
    _address = address;
}

void Server::setAddrSize(socklen_t addrSize)
{
    _addr_size = addrSize;
}

void Server::setSocket(int socket)
{
    _listenSocket = socket;
}


void    Server::addUserToChannel(User& user, Channel channel)
{
    Channel*    chan;
    try
    {
        chan = &getChannel(_channels, chaname);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';

        chan = new Channel(chaname);
        _channels.push_back( chan );
    }

    chan->userAdd(user);    
}

void    Server::rmUserFromChannel(User& user, Channel channel)
{
    Channel*    chan;
    try
    {
        chan = &getChannel(_channels, chaname);
        chan->userRemove(user);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void    Server::destroyChannel(Channel& channel)
{
    channel.usersDrop();
    _channels.remove(&channel);
    delete &channel;
}