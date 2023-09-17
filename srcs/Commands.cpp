#include "Server.hpp"

void Server::execute(std::list<ServerMessage> messageList)
{
	std::list<ServerMessage>::iterator it = messageList.begin();
    _stop = false;
    for (; it != messageList.end() && _stop == false; it++)
	{
		std::string command = it->getCommand();
        std::cout << "Received command: " << command << std::endl;
		if (_commandMap.find(command) != _commandMap.end())
		{
			(this->*_commandMap[command])(*it);
		}
		else
		{
			std::cout << "Command not found" << messageList.front().getCommand() << std::endl; // needs to send numeric?
        }
	}
}

void Server::cap(ServerMessage serverMessage)
{
    // The server does not provide capability negotiation I THINK
    //  OR maybe this is needed to do the file transfer, need to check
    // https://ircv3.net/specs/extensions/capability-negotiation.html
    std::cout << "cap command" << std::endl;
}

void Server::pass(ServerMessage serverMessage)
{
    if (_users.find(serverMessage.getSocket())->second.isLoggedIn())
    {
        send(serverMessage.getSocket(), ERR_ALREADYREGISTRED.c_str(), ERR_ALREADYREGISTRED.size(), 0);
        _stop = true;
        return;
    }

    if (serverMessage.getParams().size() < 1)
    {
        send(serverMessage.getSocket(), ERR_NEEDMOREPARAMS("PASS").c_str(), ERR_NEEDMOREPARAMS("PASS").size(), 0);
        _stop = true;
        return;
    }

    if (serverMessage.getParams()[0] != _password)
    {
        send(serverMessage.getSocket(), ERR_PASSWDMISMATCH.c_str(), ERR_PASSWDMISMATCH.size(), 0);
        _stop = true;
        // remove user from map
        return;
    }

    _users[serverMessage.getSocket()].setIsLoggedIn(true);
    send(serverMessage.getSocket(), RPL_WELCOME(_users[serverMessage.getSocket()].getNickname()).c_str(), RPL_WELCOME(_users[serverMessage.getSocket()].getNickname()).size(), 0);
    std::cout << "password verified" << std::endl;
}

// Done
void Server::nick(ServerMessage serverMessage)
{
    if (serverMessage.getParams().size() < 1)
    {
        send(serverMessage.getSocket(), ERR_NONICKNAMEGIVEN.c_str(), ERR_NONICKNAMEGIVEN.size(), 0);
        _stop = true;
        // remove user from map
        return;
    }
    
    if (serverMessage.getParams()[0].size() < 4 || serverMessage.getParams()[0].size() > 16)
    {
        send(serverMessage.getSocket(), ERR_ERRONEUSNICKNAME(serverMessage.getParams()[0], "Wrong nickname size (use between 4 and 16)").c_str(), ERR_ERRONEUSNICKNAME(serverMessage.getParams()[0], "Wrong nickname size (use between 4 and 16)").size(), 0);
        _stop = true;
        // remove user from map
        return;
    }

    std::map<int, User>::iterator it = _users.begin();
    if(serverMessage.getParams()[0].find_first_not_of(ACCEPTED_CHARS_NAME) != std::string::npos)
    {
        // I can log in using Hexchat but not using nc, it quits here. error might come from find_first_not_of??
        send(serverMessage.getSocket(), ERR_ERRONEUSNICKNAME(serverMessage.getParams()[0], "Invalid character(s) used").c_str(), ERR_ERRONEUSNICKNAME(serverMessage.getParams()[0], "Invalid character(s) used").size(), 0);
        std::cout << serverMessage.getParams()[0].find_first_not_of(ACCEPTED_CHARS_NAME) << std::endl;
        _stop = true;
        // remove user from map
        return;
    }

    // Is nick in use
    for (; it != _users.end(); it++)
    {
        if (it->second.getNickname() == serverMessage.getParams()[0])
        {
            send(serverMessage.getSocket(), ERR_NICKNAMEINUSE(serverMessage.getParams()[0]).c_str(), ERR_NICKNAMEINUSE(serverMessage.getParams()[0]).size(), 0);
            // need to remove both users from map
            _stop = true;
            return;
        }
    }
    
    // If not in use, set nickname
    _users[serverMessage.getSocket()].setNickname(serverMessage.getParams()[0]);
    std::cout << "new nickname: " << _users[serverMessage.getSocket()].getNickname() << std::endl;
}

void Server::user(ServerMessage serverMessage)
{
    if (serverMessage.getParams().size() != 4)
    {
        send(serverMessage.getSocket(), ERR_NEEDMOREPARAMS("USER").c_str(), ERR_NEEDMOREPARAMS("USER").size(), 0);
        _stop = true;
        return;
    }

    std::map<int, User>::iterator it = _users.begin();
    if(serverMessage.getParams()[0].find_first_not_of(ACCEPTED_CHARS_NAME) != std::string::npos)
    {
        send(serverMessage.getSocket(), ERR_ERRONEUSNICKNAME(serverMessage.getParams()[0], "Invalid character(s) used").c_str(), ERR_ERRONEUSNICKNAME(serverMessage.getParams()[0], "Invalid character(s) used").size(), 0);
        _stop = true;
        return;
    }

    // Is user in use
    for (; it != _users.end(); it++)
    {
        if (it->second.getUsername() == serverMessage.getParams()[0])
        {
            // need to remove both users from map
            std::cout << "user in use" << std::endl;
            send(serverMessage.getSocket(), ERR_ALREADYREGISTRED.c_str(), ERR_ALREADYREGISTRED.size(), 0);
            _stop = true;
            return;
        }
    }
    
    // If not in use, set username
    _users[serverMessage.getSocket()].setUsername(serverMessage.getParams()[0]);
    std::cout << "new username: " << _users[serverMessage.getSocket()].getUsername() << std::endl;
}

void Server::privmsg(ServerMessage serverMessage)
{
    std::cout << "privmsg command" << std::endl;


    // Are we supposed to allow <me> to send to <me>? Ex: dadoming to dadoming ?


    // Check input
    if (serverMessage.getParams().size() < 2)
    {
        send(serverMessage.getSocket(), ERR_NEEDMOREPARAMS("PRIVMSG").c_str(), ERR_NEEDMOREPARAMS("PRIVMSG").size(), 0);
        std::string usage("PRIVMSG <receiver nickname> <message>");
        send(serverMessage.getSocket(), usage.c_str(), usage.size(), 0);
        _stop = true;
        return;
    }

    // Find nickname in server
    std::map<int, User>::iterator it = _users.begin();
    for (; it != _users.end(); it++)
    {
        if (it->second.getNickname() == serverMessage.getParams()[0])
        {
            std::string message = ":" + _users[serverMessage.getSocket()].getNickname() + "!" + _users[serverMessage.getSocket()].getUsername() + "@" + SERVER_NAME + " PRIVMSG " + serverMessage.getParams()[0] + " :" + serverMessage.getParams()[1] + "\r\n";
            send(it->first, message.c_str(), message.size(), 0);
            return;
        }
    }

    // If not found
    send(serverMessage.getSocket(), ERR_NOSUCHNICK(serverMessage.getParams()[0]).c_str(), ERR_NOSUCHNICK(serverMessage.getParams()[0]).size(), 0);
}

void Server::oper(ServerMessage serverMessage)
{
    std::cout << "oper command" << std::endl;

    // Check input
    if (serverMessage.getParams().size() != 2)
    {
        send(serverMessage.getSocket(), ERR_NEEDMOREPARAMS("OPER").c_str(), ERR_NEEDMOREPARAMS("OPER").size(), 0);
        std::string usage("OPER <username> <password>");
        send(serverMessage.getSocket(), usage.c_str(), usage.size(), 0);
        _stop = true;
        return;
    }

    // find user in database
    std::map<int, User>::iterator it = _users.begin();
    for (; it != _users.end(); it++)
    {
        if (it->second.getUsername() == serverMessage.getParams()[0])
        {
            if (it->second.isOperator() == false && serverMessage.getParams()[1] == _password)
            {
                    // bug: not fully displayed
                    // display mode message
                it->second.setIsOperator(true);
                send(it->second.isOperator(), RPL_YOUREOPER.c_str(), RPL_YOUREOPER.size(), 0);
                return;
            }
            else if (it->second.isOperator() == true)
            {
                std::string msg(_users[serverMessage.getSocket()].getUsername() + "already is an operator.\r\n");
                send(serverMessage.getSocket(), msg.c_str(), msg.size(), 0);
                return;
            }
            else
            {
                send(serverMessage.getSocket(), ERR_PASSWDMISMATCH.c_str(), ERR_PASSWDMISMATCH.size(), 0);
                _stop = true;
                return;
            }
        }
    }

    // If not found
    std::string msg("Couldn't find " + serverMessage.getParams()[0] + "\r\n");
                send(serverMessage.getSocket(), msg.c_str(), msg.size(), 0);

}

void Server::quit(ServerMessage serverMessage)
{
    std::cout << "quit command" << std::endl;

    //  or maybe remove users from channels and remove user from map memory
}

void Server::mode(ServerMessage serverMessage)
{
    std::cout << "mode command" << std::endl;
}

/*  CHANNEL OPERATIONS  */
void Server::join(ServerMessage serverMessage)
{
    std::cout << "join command" << std::endl;
    User&   joiner = getUserBySocket(serverMessage.getSocket());
    const std::string chaname = trim(serverMessage.getParams()[0]);
    Channel& channel = _channels.find(chaname)->second;
    
    // Actually add
    userAddToChannel(joiner, chaname);
    
    // JOIN message
    std::string msg = ":" + joiner.getNickname() + " JOIN " + chaname + "\r\n";
    std::cout << msg;
    sendMsg(serverMessage.getSocket(), msg);

    // 332
    msg = joiner.getNickname() + " " + chaname + " :" + channel.getTopic() + "\r\n";
    std::cout << msg;
    sendMsg(serverMessage.getSocket(), msg);

    // 353
    //TODO: apply correct symbol
    msg = joiner.getNickname() + " = " + chaname + " :" + channel.getUsers() + "\r\n";
    std::cout << msg;
    sendMsg(serverMessage.getSocket(), msg);

    // 366
    msg = joiner.getNickname() + " " + chaname + " :End of /NAMES list\r\n";
    std::cout << msg;
    sendMsg(serverMessage.getSocket(), msg);    
}

void Server::part(ServerMessage serverMessage)
{
    std::cout << "part command" << std::endl;
    serverMessage.outputPrompt();
    User&   parter = getUserBySocket(serverMessage.getSocket());
    userRmFromChannel(parter, serverMessage.getParams()[0]);
}

void Server::topic(ServerMessage serverMessage)
{
    std::cout << "topic command" << std::endl;
}

void Server::names(ServerMessage serverMessage)
{
    std::cout << "name command" << std::endl;
}

void Server::list(ServerMessage serverMessage)
{
    std::cout << "list command" << std::endl;
}

void Server::invite(ServerMessage serverMessage)
{
    std::cout << "invite command" << std::endl;
}

void Server::kick(ServerMessage serverMessage)
{
    std::cout << "kick command" << std::endl;
}

