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
            it->outputPrompt();
			(this->*_commandMap[command])(*it);
		}
		else
		{
			std::cout << "Command not found" << std::endl; // needs to send numeric?
            it->outputPrompt();
        }
	}
}

void Server::cap(ServerMessage serverMessage)
{
    // The server does not provide capability negotiation I THINK
    //  OR maybe this is needed to do the file transfer, need to check
    // https://ircv3.net/specs/extensions/capability-negotiation.html

    // Caps: filetransfer (https://ircv3.net/specs/extensions/file-transfer-3.2.html)

}

void Server::sendErrorMessage(int socket, std::string error, std::string extraMessage, bool toStop)
{
    send(socket , error.c_str(), error.size(), 0);
    std::cout << RED << "fd: " << socket << " -> " << error << RESET << std::endl;
    if (extraMessage != "")
    {
        extraMessage += "\r\n";
        send(socket, extraMessage.c_str(), extraMessage.size(), 0);
    }
    if (toStop)
        _stop = true;
}

void Server::sendSuccessMessage(int socket, std::string numeric, std::string extraMessage)
{
    send(socket , numeric.c_str(), numeric.size(), 0);
    std::cout << GREEN << "fd: " << socket << " -> " << numeric << RESET << std::endl;
    if (extraMessage != "")
        send(socket, extraMessage.c_str(), extraMessage.size(), 0);
}

void Server::pass(ServerMessage serverMessage)
{
    User& user = _users[serverMessage.getSocket()];
    
    // Already logged in
    if (user.isLoggedIn())
    {
        sendErrorMessage(user.getSocket(), ERR_ALREADYREGISTRED, "", true);
        return;
    }

    // No password given
    if (serverMessage.getParams().empty())
    {
        sendErrorMessage(user.getSocket(), ERR_NEEDMOREPARAMS("PASS"), "Usage: /PASS <password>", true);
        return;
    }

    // Password mismatch
    if (serverMessage.getParams()[0] != _password)
    {
        sendErrorMessage(user.getSocket(), ERR_PASSWDMISMATCH, "", true);
        return;
    }

    // Invalid Credentials
    if (user.getNickname() == "" || user.getUsername() == "")
    {
        sendErrorMessage(user.getSocket(), ERR_ALREADYREGISTRED, "Nickname or Username is currently being used.", true);
        return;
    }
    
    user.setIsLoggedIn(true);
    sendSuccessMessage(user.getSocket(), RPL_WELCOME(user.getNickname()), ""); // TODO: add extra message
    std::cout << YELLOW << "> password verified <" << RESET << std::endl;
}

void Server::nick(ServerMessage serverMessage)
{
    User& user = _users[serverMessage.getSocket()];

    if (user.isLoggedIn())
    {
        sendErrorMessage(user.getSocket(), ERR_ALREADYREGISTRED, "", true);
        return;
    }

    if (serverMessage.getParams().empty())
    {
        sendErrorMessage(user.getSocket(), ERR_NONICKNAMEGIVEN, "", true);
        return;
    }

    std::string newNick = serverMessage.getParams()[0];
    if(newNick.find_first_not_of(ACCEPTED_CHARS_NAME) != std::string::npos)
    {
        // I can log in using Hexchat but not using nc, it quits here. error might come from find_first_not_of??
        sendErrorMessage(user.getSocket(), ERR_ERRONEUSNICKNAME(newNick, "Invalid character(s) used"), "", true);
        return;
    }

    std::map<int, User>::iterator it = _users.begin();
    for (; it != _users.end(); it++)
    {
        if (it->second.getNickname() == newNick)
        {
            sendErrorMessage(user.getSocket(), ERR_NICKNAMEINUSE(newNick), "", true);
            return;
        }
    }
    
    user.setNickname(newNick);
    std::cout << GREEN << "new nickname: " << user.getNickname() << RESET << std::endl;
}

void Server::user(ServerMessage serverMessage)
{
    User& user = _users[serverMessage.getSocket()];

    if (user.isLoggedIn())
    {
        sendErrorMessage(user.getSocket(), ERR_ALREADYREGISTRED, "", true);
        return;
    }

    if (serverMessage.getParams().size() != 4)
    {
        sendErrorMessage(user.getSocket(), ERR_NEEDMOREPARAMS("USER"), "Usage: /USER <username> <hostname> <servername> <realname>", true);
        return;
    }

    std::string newUser = serverMessage.getParams()[0];
    if(newUser.find_first_not_of(ACCEPTED_CHARS_NAME) != std::string::npos)
    {
        sendErrorMessage(user.getSocket(), ERR_ERRONEUSNICKNAME(newUser, "Invalid character(s) used"), "", true);
        return;
    }

    std::map<int, User>::iterator it = _users.begin();
    for (; it != _users.end(); it++)
    {
        if (it->second.getUsername() == newUser)
        {
            sendErrorMessage(user.getSocket(), ERR_NICKNAMEINUSE(newUser), "", true);
            return;
        }
    }
    
    user.setUsername(newUser);
    std::cout << GREEN << "new username: " << user.getUsername() << RESET << std::endl;
}

void Server::privmsg(ServerMessage serverMessage)
{
    // Are we supposed to allow <me> to send to <me>? Ex: dadoming to dadoming ?

    // Check input
    if (serverMessage.getParams().size() < 2)
    {
        sendErrorMessage(serverMessage.getSocket(), ERR_NEEDMOREPARAMS("PRIVMSG"), "Usage: /PRIVMSG <receiver>{,<receiver>} <message>", false);
        return;
    }

    std::vector<std::string> receivers = split(serverMessage.getParams()[0], ",");
    std::string message = serverMessage.getParams()[1];

    // Swap vector to list for easier removal
    std::list<std::string> receiversList(receivers.begin(), receivers.end());
    for(std::list<std::string>::iterator it = receiversList.begin(); it != receiversList.end(); it++)
    {
        // Check if receiver is a channel
        std::map<std::string, Channel>::iterator chan = _channels.find(*it);
        if (chan != _channels.end())
        {
            // Send message to all users in channel
            std::map<int, User*>::iterator user = chan->second.getUsers().begin();
            for (; user != chan->second.getUsers().end(); user++)
            {
                sendSuccessMessage(user->first, PRIVMSG(_users[serverMessage.getSocket()].getNickname(), chan->first, message), "");
            }
        }
        else
        {
            // Check if receiver is a user
            std::map<int, User>::iterator user = _users.begin();
            for (; user != _users.end(); user++)
            {
                if (user->second.getNickname() == *it)
                {
                    sendSuccessMessage(user->first, PRIVMSG(_users[serverMessage.getSocket()].getNickname(), user->second.getNickname(), message), "");
                    break;
                }
            }
            // If not found
            if (user == _users.end())
                sendErrorMessage(serverMessage.getSocket(), ERR_NOSUCHNICKCHAN(_users[serverMessage.getSocket()].getNickname(), *it), "", false);
        }
    }
}


/*

   If the client sending the OPER command supplies the correct password
   for the given user, the server then informs the rest of the network
   of the new operator by issuing a "MODE +o" for the clients nickname.

   The OPER message is client-server only.

*/

void Server::oper(ServerMessage serverMessage)
{
    // Check input
    if (serverMessage.getParams().size() != 2)
    {
        sendErrorMessage(serverMessage.getSocket(), ERR_NEEDMOREPARAMS("OPER"), "Usage: /OPER <username> <password>", false);
        return;
    }

    // find user in database
    std::map<int, User>::iterator it = _users.begin();
    for (; it != _users.end(); it++)
    {
        if (it->second.getUsername() == serverMessage.getParams()[0])
        {
            if (it->second.isOperator() == false && serverMessage.getParams()[1] == OPERATOR_PASSWORD)
            {
                    // bug: not fully displayed
                    // display mode message
                it->second.setIsOperator(true);
                sendSuccessMessage(serverMessage.getSocket(), RPL_YOUREOPER, "");
                return;
            }
            else if (it->second.isOperator() == true)
            {
                sendErrorMessage(serverMessage.getSocket(), ERR_ALREADYREGISTRED, "", false);
                return;
            }
            else
            {
                sendErrorMessage(serverMessage.getSocket(), ERR_PASSWDMISMATCH, "", false);
                return;
            }
        }
    }

    // If not found
    sendErrorMessage(serverMessage.getSocket(), ERR_NOSUCHNICK(serverMessage.getParams()[0]), "", false);
}

void Server::quit(ServerMessage serverMessage)
{
    // Remove user from all channels
    std::map<std::string, Channel>::iterator chan = _channels.begin();
    for (; chan != _channels.end(); chan++)
    {
        chan->second.userRemove(_users[serverMessage.getSocket()]);
    }
}

void Server::mode(ServerMessage serverMessage)
{
    std::cout << "mode command" << std::endl;
}

/*  CHANNEL OPERATIONS  */
void Server::join(ServerMessage serverMessage)
{

    //// Server join is REQUIRED to have first char of channel name to be '#' or '&'!
//
    //std::cout << "join command" << std::endl;
//
    //// Check input
    //if (serverMessage.getParams().size() < 1)
    //{
    //    send(serverMessage.getSocket(), ERR_NEEDMOREPARAMS("JOIN").c_str(), ERR_NEEDMOREPARAMS("JOIN").size(), 0);
    //    std::string usage("JOIN <channel name>");
    //    send(serverMessage.getSocket(), usage.c_str(), usage.size(), 0);
    //    _stop = true;
    //    return;
    //}
//
    //// Check if channel exists, if not create it and add user to it
    //userAddToChannel(_users[serverMessage.getSocket()], serverMessage.getParams()[0]);
    //std::string success(":" + _users[serverMessage.getSocket()].getNickname() + "!" + _users[serverMessage.getSocket()].getUsername() + "@" + SERVER_NAME + " JOIN " + serverMessage.getParams()[0] + "\r\n");
    //send(serverMessage.getSocket(), success.c_str(), success.size(), 0);
//
    //// Send names list TODO
    //// std::map<std::string, Channel>::iterator it = _channels.begin();
    std::cout << "join command" << std::endl;
    serverMessage.outputPrompt();

    User&   joiner = getUserBySocket(serverMessage.getSocket());
    const std::string chaname = serverMessage.getParams()[0];
    Channel& channel = _channels.find(chaname)->second;
    
    std::cout << (int)chaname[6] << std::endl;

    // Actually add
    userAddToChannel(joiner, chaname);
    
    // JOIN message
    std::string msg = ":" + joiner.getNickname() + " JOIN " + chaname;
    std::cout << msg;
    sendMsg(serverMessage.getSocket(), msg + "\r\n");

    // 332
    msg = joiner.getNickname() + " " + chaname + " :" + channel.getTopic();
    // std::cout << msg;
    sendMsg(serverMessage.getSocket(), msg + "\r\n");

    // 353
    //TODO: apply correct symbol
    msg = joiner.getNickname() + " = " + chaname + " :" + channel.getUsersString();
    std::cout << msg;
    sendMsg(serverMessage.getSocket(), msg + "\r\n");

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

