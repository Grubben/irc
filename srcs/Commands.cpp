#include "Server.hpp"

void Server::execute(std::list<ServerMessage> messageList)
{
	std::list<ServerMessage>::iterator it = messageList.begin();
    _stop = false;
    for (; it != messageList.end() && _stop == false; it++)
	{
		std::string command = it->getCommand();
        it->outputPrompt();
		try
        {
            if (_commandMap.find(command) != _commandMap.end())
			    (this->*_commandMap[command])(*it);
        }
        catch(std::string error)
        {
            std::cout << error << std::cout;
            sendErrorMessage(it->getSocket(), error);
        }
        catch(std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
	}
}

void Server::cap(ServerMessage serverMessage)
{
    (void) serverMessage;
    // We don't support CAP
}

void Server::sendErrorMessage(int socket, std::string error)
{
    sendAll(socket , error);
    std::cout << RED << "fd: " << socket << " -> " << error << RESET << std::endl;
}

void Server::sendSuccessMessage(int socket, std::string numeric, std::string extraMessage)
{
    sendAll(socket , numeric);
    std::cout << GREEN << "fd: " << socket << " -> " << numeric << RESET << std::endl;
    if (extraMessage != "")
        sendAll(socket, extraMessage);
}

void Server::pass(ServerMessage serverMessage)
{
    User& user = _users[serverMessage.getSocket()];
    
    if (user.isLoggedIn())
        throw std::string(ERR_ALREADYREGISTRED);

    if (serverMessage.getParams().empty())
        throw std::string(ERR_NEEDMOREPARAMS("PASS"));

    if (serverMessage.getParams()[0] != _password)
        throw std::string(ERR_PASSWDMISMATCH);

    if (user.getNickname() == "")
        throw std::string(ERR_NOSUCHNICK(user.getNickname()));
    else if (user.getUsername() == "")
        throw std::string(ERR_NOSUCHNICK(user.getUsername()));
    
    user.setIsLoggedIn(true);
    sendSuccessMessage(user.getSocket(), RPL_WELCOME(user.getNickname()), ""); // TODO: add extra message
    sendSuccessMessage(user.getSocket(), RPL_YOURHOST(user.getNickname()), "");
    std::cout << YELLOW << "> password verified <" << RESET << std::endl;
}

void Server::nick(ServerMessage serverMessage)
{
    //TODO: send numeric macro to change nick
    User& user = _users[serverMessage.getSocket()];

    if (serverMessage.getParams().empty())
        throw std::string(ERR_NONICKNAMEGIVEN);

    std::string newNick = serverMessage.getParams()[0];
    if(newNick.find_first_not_of(ACCEPTED_CHARS_NAME) != std::string::npos)
        throw std::string(ERR_ERRONEUSNICKNAME(newNick, "Invalid character(s) used"));

    std::map<int, User>::iterator it = _users.begin();
    for (; it != _users.end(); it++)
    {
        if (it->second.getNickname() == newNick)
            throw std::string(ERR_NICKNAMEINUSE(newNick));
    }
    
    user.setNickname(newNick);
    sendSuccessMessage(user.getSocket(), RPL_CHANGENICK(user.getNickname()), "");
    std::cout << GREEN << "new nickname: " << user.getNickname() << RESET << std::endl;
}


// Why do we always need to reset user upon login fail? VER DISTO
void Server::user(ServerMessage serverMessage)
{
    User& user = _users[serverMessage.getSocket()];

    if (user.isLoggedIn())
        throw std::string(ERR_ALREADYREGISTRED);

    if (serverMessage.getParams().size() != 4)
        throw std::string(ERR_NEEDMOREPARAMS("USER"));

    std::string newUser = serverMessage.getParams()[0];
    if(newUser.find_first_not_of(ACCEPTED_CHARS_NAME) != std::string::npos)
        throw std::string(ERR_ERRONEUSNICKNAME(newUser, "Invalid character(s) used"));
    
    user.setUsername(newUser);
    std::cout << GREEN << "new username: " << user.getUsername() << RESET << std::endl;
}

void Server::privmsg(ServerMessage serverMessage)
{
    // Are we supposed to allow <me> to send to <me>? Ex: dadoming to dadoming ?

    if (serverMessage.getParams().size() < 2)
        throw std::string(ERR_NEEDMOREPARAMS("PRIVMSG"));

    std::vector<std::string> receivers = split(serverMessage.getParams()[0], ",");
    User& sender = _users[serverMessage.getSocket()];

    //uncomment for evaluation
    //if (sender.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);

    std::string message = "";
    for (size_t i = 1; i < serverMessage.getParams().size(); i++)
        message += serverMessage.getParams()[i] + " ";

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
                if (message[0] == ':')
                    message.erase(0, 1);
                if (sender.getSocket() != user->first)
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
                    if (message[0] == ':')
                        message.erase(0, 1);
                    sendSuccessMessage(user->first, PRIVMSG(sender.getNickname(), user->second.getNickname(), message), "");
                    break;
                }
            }
            // If not found
            if (user == _users.end())
                sendErrorMessage(serverMessage.getSocket(), ERR_NOSUCHNICKCHAN(_users[serverMessage.getSocket()].getNickname(), *it));
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
    //uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);

    if (serverMessage.getParams().size() != 2)
        throw ERR_NEEDMOREPARAMS("OPER");

    std::map<int, User>::iterator it = _users.begin();
    for (; it != _users.end(); it++)
    {
        if (it->second.getUsername() == serverMessage.getParams()[0])
        {
            if (it->second.isOperator() == false && serverMessage.getParams()[1] == OPERATOR_PASSWORD)
            {
                it->second.setIsOperator(true);
                sendSuccessMessage(serverMessage.getSocket(), RPL_YOUREOPER, "");
                return;
            }
            else if (it->second.isOperator() == true)
                throw std::string(ERR_ALREADYREGISTRED);
            else
                throw std::string(ERR_PASSWDMISMATCH);
        }
    }

    // If not found
    throw ERR_NOSUCHNICK(serverMessage.getParams()[0]);
}

void Server::quit(ServerMessage serverMessage)
{
    std::map<std::string, Channel>::iterator chan = _channels.begin();
    std::string quitNick = _users[serverMessage.getSocket()].getNickname();
    for (; chan != _channels.end(); chan++)
    {
        chan->second.userRemove(_users[serverMessage.getSocket()]);
    }
    sendSuccessMessage(serverMessage.getSocket(), QUIT(quitNick), "");
}

/*  CHANNEL OPERATIONS  */
void Server::join(ServerMessage serverMessage)
{
   //uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);

    if (serverMessage.getParams().size() < 1)
        throw std::string(ERR_NEEDMOREPARAMS("JOIN"));

    User&   joiner = getUserBySocket(serverMessage.getSocket());
    const std::string chaname = serverMessage.getParams()[0];
    if (chaname.find_first_of("&#") != 0)
        throw std::string(ERR_NOSUCHCHANNEL(chaname));

    std::map<std::string,Channel>::iterator chan = _channels.find(chaname);
    if (chan == _channels.end())
        _channels.insert(std::pair<std::string,Channel>(chaname, Channel(*this, chaname)));
    else
    {
        Channel& channel = _channels.find(chaname)->second;
        if (channel.isInviteOnly() && channel.isInvited(joiner.getNickname()) == false)
        throw std::string(ERR_INVITEONLYCHAN(joiner.getNickname(), chaname));
    
        if (channel.hasPassword() && serverMessage.getParams().size() > 1 && channel.getPassword() != serverMessage.getParams()[1])
            throw std::string(ERR_BADCHANNELKEY(joiner.getNickname(), chaname));

        if (channel.hasUserLimit() && static_cast<int>(channel.getUsers().size()) >= channel.getMaxUsers())
            throw std::string(ERR_CHANNELISFULL(joiner.getNickname(), chaname));

        if (channel.isInviteOnly() && channel.isInvited(joiner.getNickname()) == true)
        channel.removeInvited(joiner.getNickname());
    }
    Channel& channel = _channels.find(chaname)->second;

    joiner.channelJoin(channel);
    channel.userAdd(joiner);

    sendSuccessMessage(joiner.getSocket(), JOIN(joiner.getNickname(), chaname), "");
    sendSuccessMessage(joiner.getSocket(), RPL_TOPIC(joiner.getNickname() , chaname, channel.getTopic()), "");    
    sendSuccessMessage(joiner.getSocket(), RPL_NAMREPLY(joiner.getNickname(), chaname, channel.getUsersString()), "");
    sendSuccessMessage(joiner.getSocket(), RPL_ENDOFNAMES(joiner.getNickname(), chaname), "");
    if (channel.getUsers().size() == 1)
    {
        channel.addOperator(joiner);
        sendSuccessMessage(joiner.getSocket(), MODE(chaname, channel.getModes()), "");
        sendSuccessMessage(joiner.getSocket(), MODE(chaname, "+o " + joiner.getNickname()), "");
    }
}

void Server::part(ServerMessage serverMessage)
{
    User&   parter = getUserBySocket(serverMessage.getSocket());
    
    // uncomment for evaluation
    //if (parter.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);
 
    std::vector<std::string> channelsLeave = split(serverMessage.getParams()[0], ",");

    for (std::vector<std::string>::iterator it = channelsLeave.begin(); it != channelsLeave.end(); it++ )
    {
        if (! channelExists(*it))
            throw std::string(ERR_NOSUCHCHANNEL(*it));
        else if (! _channels.find(*it)->second.isUserInChannel(parter))
            throw std::string(ERR_NOTONCHANNEL(*it));
        else
        {
            // Broadcast message to all users in channel that this user left channel
            sendSuccessMessage(parter.getSocket(), PART(parter.getNickname(), *it, ""), "");
            userRmFromChannel(parter, *it);
        }
    }
}

void Server::topic(ServerMessage serverMessage)
{
    // uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);
    

    // Missing:
    //   ERR_CHANOPRIVSNEEDED -> Mode +t

    // Need to implement broadcast function and use it for this command when topic is changed

    if (serverMessage.getParams().size() < 1)
        throw std::string(ERR_NEEDMOREPARAMS("TOPIC"));

    const std::string chaname = serverMessage.getParams()[0];
    if (! channelExists(chaname))
        throw std::string(ERR_NOSUCHCHANNEL(chaname));
    
    User&   user = getUserBySocket(serverMessage.getSocket());
    if (! _channels.find(chaname)->second.isUserInChannel(user))
        throw std::string(ERR_NOTONCHANNEL(chaname));

    if (serverMessage.getParams().size() == 1)
    {
        const std::string&    topic = _channels.find(chaname)->second.getTopic();
        if (topic == "")
            sendSuccessMessage(user.getSocket(), RPL_NOTOPIC(user.getNickname(), chaname), "");
        else
            sendSuccessMessage(user.getSocket(), RPL_TOPIC(user.getNickname(), chaname, topic), "");
    }
    else {
    // didnt touch this, if topic changed then it should be broadcasted to all users in channel
    const std::string newtopic = serverMessage.getParams()[1].substr(1);
    _channels.find(chaname)->second.setTopic(newtopic);

    }
    //TODO: check permissions
    //TODO: check serverMessage.getParams()[1][0] is a ":"

}

void Server::names(ServerMessage serverMessage)
{
    User&   user = getUserBySocket(serverMessage.getSocket());

    // uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);

    if (serverMessage.getParams().size() < 1)
        throw std::string(ERR_NEEDMOREPARAMS("NAMES"));

    // Outside of channels
    if (serverMessage.getParams().size() && serverMessage.getParams()[0] == "IRC")
    {
        // TODO: Fill 'all' with list of users in all channels
        sendSuccessMessage(user.getSocket(), RPL_NAMREPLY(user.getNickname(), "*", "all"), "");
        sendSuccessMessage(user.getSocket(), RPL_ENDOFNAMES(user.getNickname(), "*"), "");
        return;
    }

    std::vector<std::string> chanames = split(serverMessage.getParams()[0], ",");
    for (std::vector<std::string>::iterator it = chanames.begin(); it != chanames.end(); it++)
    {
        if (! channelExists(*it))
            continue;
        else if (_channels.find(*it) != _channels.end())
        {
            sendSuccessMessage(user.getSocket(), RPL_NAMREPLY(user.getNickname(), *it, _channels.find(*it)->second.getUsersString()), "");
            sendSuccessMessage(user.getSocket(), RPL_ENDOFNAMES(user.getNickname(), *it), "");
        }
    }
}

void    Server::rpl_list(User& user)
{
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
    {
        std::stringstream itos;
        itos << it->second.getUsers().size();
        std::string clientCount = itos.str();
        itos.clear();
        sendSuccessMessage(user.getSocket(), RPL_LIST(user.getNickname(), it->first, clientCount, "TOPIC"), "");
    }
}

void Server::list(ServerMessage serverMessage)
{
    // uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);
    User&   user = getUserBySocket(serverMessage.getSocket());
    sendSuccessMessage(user.getSocket(), RPL_LISTSTART(user.getNickname()), "");
    rpl_list(user);
    sendSuccessMessage(user.getSocket(), RPL_LISTEND(user.getNickname()), "");
}

void Server::invite(ServerMessage serverMessage)
{
    // uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);
    if (serverMessage.getParams().size() < 2)
        throw std::string(ERR_NEEDMOREPARAMS("INVITE"));

    User&   user = getUserBySocket(serverMessage.getSocket());
    
    const std::string invited = serverMessage.getParams()[0];
    const std::string chaname = serverMessage.getParams()[1];

    if (!channelExists(chaname))
        throw std::string(ERR_NOSUCHCHANNEL(chaname));
    
    Channel& channel = _channels.find(chaname)->second;
    std::map<int, User>::iterator it = _users.begin();
    for (; it != _users.end(); it++)
    {
        if (it->second.getNickname() == invited)
            break;
    }

    if (it == _users.end() || it->second.isLoggedIn() == false)
        throw std::string(ERR_NOSUCHNICK(invited));

    if (channel.isUserInChannel(user) == false)
        throw std::string(ERR_NOTONCHANNEL(chaname));

    User&   invitedUser = it->second;
    if (channel.isUserInChannel(invitedUser))
        throw std::string(ERR_USERONCHANNEL(invited, chaname));

    if (channel.isInviteOnly())
        channel.addInvited(invitedUser.getNickname());
    sendSuccessMessage(user.getSocket(), RPL_INVITING(user.getNickname(), invited, chaname), "");
    sendSuccessMessage(it->second.getSocket(), INVITE(user.getNickname(), invited, chaname), "");
}

void Server::kick(ServerMessage serverMessage)
{
    // uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);

    if (serverMessage.getParams().size() < 2)
        throw std::string(ERR_NEEDMOREPARAMS("KICK"));

    const std::string chaname = serverMessage.getParams()[0];
    const std::string kickedName = serverMessage.getParams()[1];
    std::string reason = "";
    if (serverMessage.getParams().size() > 2)
    {
        reason = serverMessage.getParams()[2];
        if (reason[0] == ':')
            reason.erase(0, 1);
    }

    if (! channelExists(chaname))
        throw std::string(ERR_NOSUCHCHANNEL(chaname));

    Channel& channel = _channels.find(chaname)->second;
    User&   user = getUserBySocket(serverMessage.getSocket());
    if (channel.isOperator(user) == false)
        throw std::string(ERR_CHANOPRIVSNEEDED(user.getNickname(), chaname));
    
    User& kickedUser = channel.getUserByNickname(kickedName);
    if  (channel.isUserInChannel(kickedUser) == false)
        throw std::string(ERR_NOTONCHANNEL(chaname));
    userRmFromChannel(kickedUser, chaname);
    if (channel.isInvited(kickedUser.getNickname()))
        channel.removeInvited(kickedUser.getNickname());
    if (channel.getUsers().size() == 0)
        _channels.erase(chaname);
    sendSuccessMessage(user.getSocket(), KICK(user.getNickname(), chaname, kickedName), "");
    sendSuccessMessage(kickedUser.getSocket(), KICK(user.getNickname(), chaname, kickedName), "");
}

/*

    RPL_WHOREPLY (352) 
  "<client> <channel> <username> <host> <server> <nick> <flags> :<hopcount> <realname>"
Sent as a reply to the WHO command, this numeric gives information about the client with the nickname <nick>. Refer to RPL_WHOISUSER (311) for the meaning of the fields <username>, <host> and <realname>. <server> is the name of the server the client is connected to. If the WHO command was given a channel as the <mask> parameter, then the same channel MUST be returned in <channel>. Otherwise <channel> is an arbitrary channel the client is joined to or a literal asterisk character ('*', 0x2A) if no channel is returned. <hopcount> is the number of intermediate servers between the client issuing the WHO command and the client <nick>, it might be unreliable so clients SHOULD ignore it.

<flags> contains the following characters, in this order:

Away status: the letter H ('H', 0x48) to indicate that the user is here, or the letter G ('G', 0x47) to indicate that the user is gone.
Optionally, a literal asterisk character ('*', 0x2A) to indicate that the user is a server operator.
Optionally, the highest channel membership prefix that the client has in <channel>, if the client has one.
Optionally, one or more user mode characters and other arbitrary server-specific flags.

*/
void Server::who(ServerMessage serverMessage)
{
    // uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);
    User&   user = getUserBySocket(serverMessage.getSocket());
    Channel& channel = _channels.find(serverMessage.getParams()[0])->second;
    
    if (! channelExists(serverMessage.getParams()[0]))
        throw std::string(ERR_NOSUCHCHANNEL(serverMessage.getParams()[0]));
    if (channel.isUserInChannel(user) == false)
        throw std::string(ERR_NOTONCHANNEL(serverMessage.getParams()[0]));
    
    //sendSuccessMessage(user.getSocket(), RPL_WHOREPLY(user.getNickname(), channel.getName()), "");
    //sendSuccessMessage(user.getSocket(), RPL_ENDOFWHO(user.getNickname()), "");
}

void Server::mode(ServerMessage serverMessage)
{
    // uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);
    if (serverMessage.getParams().size() == 1)
    {
        const std::string chaname = serverMessage.getParams()[0];
        if (! channelExists(chaname))
            throw std::string(ERR_NOSUCHCHANNEL(chaname));
        
        Channel& channel = _channels.find(chaname)->second;
        sendSuccessMessage(serverMessage.getSocket(), RPL_CHANNELMODEIS(_users[serverMessage.getSocket()].getNickname(), chaname, channel.getModes()), "");
        return ;
    }
    else if (serverMessage.getParams().size() < 2)
        throw std::string(ERR_NEEDMOREPARAMS("MODE"));

    if (serverMessage.getParams()[0].find_first_of("&#") != 0)
        throw std::string(ERR_NOSUCHCHANNEL(serverMessage.getParams()[0]));
    
    const std::string chaname = serverMessage.getParams()[0];
    const std::string modes = serverMessage.getParams()[1];
    User&   user = getUserBySocket(serverMessage.getSocket());

    if (! channelExists(chaname))
        throw std::string(ERR_NOSUCHCHANNEL(chaname));

    std::map<std::string, Channel>::iterator chan = _channels.find(chaname);
    if (chan->second.isUserInChannel(user) == false)
        throw std::string(ERR_NOTONCHANNEL(chaname));
    
    Channel& channel = chan->second;
    if (channel.isOperator(user) == false)
        throw std::string(ERR_CHANOPRIVSNEEDED(user.getNickname(), chaname));
    
    if ((modes.size() != 2) || (modes[0] != '+' && modes[0] != '-') || \
        (modes[1] != 'i' && modes[1] != 't' && modes[1] != 'k' && modes[1] != 'l' && modes[1] != 'o'))
        throw std::string(ERR_UNKNOWNMODE(user.getNickname(), modes));
    
    if (serverMessage.getParams().size() == 2)
    {
        bool   sign = modes[0] == '+'; // + = 1, - = 0
        char   mode = modes[1];
        if (mode != 'i' && mode != 't' && mode != 'l')
            throw std::string(ERR_NEEDMOREPARAMS("MODE"));
        if (mode == 'i')
        {
            channel.setInviteOnly(sign);
            sendSuccessMessage(user.getSocket(), OP_MODE(user.getNickname(), chaname, modes), "");
        }
        else if (mode == 't')
        {
            channel.setTopicRestrict(sign);
            sendSuccessMessage(user.getSocket(), OP_MODE(user.getNickname(), chaname, modes), "");
        }
        else if (mode == 'l' && sign == false)
        {
            channel.setMaxUsers(sign, 0);
            sendSuccessMessage(user.getSocket(), OP_MODE(user.getNickname(), chaname, modes), "");
            return ;
        }
    }
    if (serverMessage.getParams().size() == 3)
    {
        bool   sign = modes[0] == '+';
        char   mode = modes[1];
        std::string param = serverMessage.getParams()[2];
        if (mode == 'k')
        {
            channel.setPassword(sign, param);
            sendSuccessMessage(user.getSocket(), OP_MODE(user.getNickname(), chaname, modes + " " + param), "");
        }
        else if (mode == 'l')
        {
            int maxUsers = std::atoi(param.c_str());
            if (maxUsers > 5000 || maxUsers < 0)
                throw std::string(ERR_NEEDMOREPARAMS("MODE"));
            if (maxUsers < static_cast<int>(channel.getUsers().size()))
                throw std::string(ERR_NEEDMOREPARAMS("MODE"));
            channel.setMaxUsers(sign, maxUsers);
            sendSuccessMessage(user.getSocket(), OP_MODE(user.getNickname(), chaname, modes + " " + param), "");
        }
        else if (mode == 'o')
        {
            if (sign)
                channel.addOperator(channel.getUserByNickname(param));
            else 
                channel.removeOperator(channel.getUserByNickname(param));
            sendSuccessMessage(user.getSocket(), OP_MODE(user.getNickname(), chaname, modes + " " + param), "");
        }
    }
}
