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

    /*  commands    */
    _commandMap["CAP"] = &Server::cap;
    _commandMap["PASS"] = &Server::pass;
	_commandMap["NICK"] = &Server::nick;
	_commandMap["USER"] = &Server::user;
	_commandMap["OPER"] = &Server::oper;
	_commandMap["QUIT"] = &Server::quit;
	_commandMap["JOIN"] = &Server::join;
	_commandMap["PART"] = &Server::part;
	_commandMap["MODE"] = &Server::mode;
	_commandMap["TOPIC"] = &Server::topic;
}

Server::~Server()
{
    close(_listenSocket);

    for (std::map<int, User>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        close(it->first);
    }
    _users.clear();
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
        getUserBySocket(i).says(message);
        std::cout << message;
    }

}

void    Server::userQuit(const int socket)
{
    std::cout << "Server is removing user with fd: " << socket << std::endl;

    std::map<int, User>::iterator search = _users.find(socket);

    close(search->first);
    _users.erase(search);

    std::cout << "users size: " << _users.size() << std::endl;
}

void    Server::userCreate(int socket)
{
    _users.insert(std::pair<int,User>(socket, User(*this, socket)));
}

void    Server::userAddToChannel(User& user, std::string chaname)
{
    std::map<std::string,Channel>::iterator    chan;
    try
    {
        chan = _channels.find(chaname);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        _channels.insert(std::pair<std::string,Channel>(chaname, Channel(*this, chaname)));
    }

    user.channelJoin(chan->second);
    chan->second.userAdd(user);
}

void    Server::userRmFromChannel(User& user, std::string chaname)
{
    user.channelPart(chaname);

    std::map<std::string,Channel>::iterator search = _channels.find(chaname);
    int nusers = search->second.userRemove(user);
    if (nusers == 0)
        _channels.erase(search);
}

void    Server::channelCreate(std::string chaname)
{
    _channels.insert(std::pair<std::string,Channel>(chaname, Channel(*this, chaname)));
}

// void    Server::channelDestroy(Channel& channel)
// {
//     _channels.remove(&channel);
//     delete &channel;
// }