#include "Server.hpp"

Server::Server(std::string port, std::string password)
{

    _portNumber = port;
    _password = password;

    /* Set socket options to IPv4, port number and IP address. */
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = AF_INET6;
    _hints.ai_socktype = SOCK_STREAM;
    _hints.ai_flags = AI_PASSIVE;
    
    addrinfo *ai, *p;
    int rv = getaddrinfo(NULL, _portNumber.c_str(), &_hints, &ai);
    if(rv  != 0)
    {
        throw SocketInitializationError();
    }
    for(p = ai; p != NULL; p = p->ai_next)
    {
        _listenSocket = socket(p->ai_family, p->ai_socktype, getprotobyname("TCP")->p_proto);
        if (_listenSocket < 0)
            continue;
        
        int yes = 1;
        setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    
        if (bind(_listenSocket, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(_listenSocket);
            continue;
        }

        break;
    }
    
    if (p == NULL)
    {
        std::cout << "No connect" << std::endl;
        exit(2);
    }

    freeaddrinfo(ai);    

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
    _commandMap["PRIVMSG"] = &Server::privmsg;
    //Channel Operations
	_commandMap["JOIN"] = &Server::join;
	_commandMap["PART"] = &Server::part;
	_commandMap["TOPIC"] = &Server::topic;
    _commandMap["NAMES"] = &Server::names;
    _commandMap["LIST"] = &Server::list;
    _commandMap["INVITE"] = &Server::invite;
    _commandMap["KICK"] = &Server::kick;
    _commandMap["MODE"] = &Server::mode;
    _commandMap["WHO"] = &Server::who;
}

Server::~Server()
{
    close(_listenSocket);

    std::cout << "Closing all sockets" << std::endl;

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

    g_isRunning = true;
    while (g_isRunning)
    {
        readFDs = _masterFDs;
        
        n = select(_fdMax + 1, &readFDs, NULL, NULL, NULL);
        if (n == -1)
            break;
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
    
    std::string passMsg = "This server requires a password. Please type /PASS <password> in order to try to log in.\r\n";
    sendSuccessMessage(newUserSocket, passMsg, "");
}

void Server::dataReceived(int i)
{
    char message[1024] = "";
    int len;
    if ((len = recv(i, message, sizeof(message) - 1, 0) <= 0))
    {
        if (len == 0)
            std::cout << "Client has left the network. fd: " << i << std::endl;
        message[len] = 0;
        //close(i);
        FD_CLR(i, &_masterFDs);
        this->userQuit(i);
    }
    else
    {
        std::string toSendMessage(message);
        User& user = getUserBySocket(i);

        if (toSendMessage.find('\n') != std::string::npos)
            user.says(user.flushBuffer() + toSendMessage, *this);
        else
            user.addBuffer(toSendMessage);
    }

}

void    Server::userQuit(const int socket)
{
    std::cout << "Server is removing user with fd: " << socket << std::endl;
    User& user = getUserBySocket(socket);
    
    std::vector<std::string> channelNames;
    std::map<std::string, Channel>::iterator s_it = _channels.begin();
    while (s_it != _channels.end())
    {
        if (s_it->second.isUserInChannel(user))
        {
            s_it->second.broadcastMessagetoChannel(QUIT(_users[socket].getNickname()), user);
            channelNames.push_back(s_it->first);
        }
        s_it++;
    }

    std::cout << "Removing user from channels" << std::endl;

    for (std::vector<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
    {
        std::cout << "Removing user from channel: " << *it << std::endl;
        userRmFromChannel(user, *it);
    }

    close(user.getSocket());
    _users.erase(user.getSocket());
    std::cout << "users size: " << _users.size() << std::endl;
}

void    Server::userCreate(int socket)
{
    _users.insert(std::pair<int, User>(socket, User( socket)));
}

void    Server::userRmFromChannel(User& user, std::string chaname)
{
    user.channelPart(chaname);

    std::map<std::string, Channel>::iterator search = _channels.find(chaname);
    if (search == _channels.end())
        return ;
    int nusers = search->second.userRemove(user);
    if (nusers == 0)
    {
        _channels.erase(search);
    }
}

bool    Server::channelExists(std::string chaname)
{
    std::map<std::string, Channel>::iterator search = _channels.find(chaname);
    return search != _channels.end();
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