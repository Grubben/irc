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
    sendSuccessMessage(user.getSocket(), NICK(user.getNickname(), newNick), "");

    // if user is at least in 1 channel, change names in all channels
    if (this->_channels.size() > 0)
    {
        std::map<std::string, Channel>::iterator chan = _channels.begin();
        for (; chan != _channels.end(); chan++)
        {
            if (chan->second.isUserInChannel(user))
            {
                sendSuccessMessage(user.getSocket(), RPL_NAMREPLY(user.getNickname(), chan->first, chan->second.getUsersString()), "");
                chan->second.broadcastMessagetoChannel(RPL_NAMREPLY(user.getNickname(), chan->first, chan->second.getUsersString()), user);
                chan->second.broadcastMessagetoChannel(RPL_ENDOFNAMES(user.getNickname(), chan->first), user);
            }
        }
    }

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
        if (channel.isInviteOnly() && channel.isInvited(joiner) == false)
        throw std::string(ERR_INVITEONLYCHAN(joiner.getNickname(), chaname));
    
        if (channel.hasPassword() && serverMessage.getParams().size() > 1 && channel.getPassword() != serverMessage.getParams()[1])
            throw std::string(ERR_BADCHANNELKEY(joiner.getNickname(), chaname));

        if (channel.hasUserLimit() && static_cast<int>(channel.getUsers().size()) >= channel.getMaxUsers())
            throw std::string(ERR_CHANNELISFULL(joiner.getNickname(), chaname));

        if (channel.isInviteOnly() && channel.isInvited(joiner) == true)
        channel.removeInvited(joiner);
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
    // broadcast to all in channel that user joined
    std::map<int, User*>::iterator it = channel.getUsers().begin();
    for (; it != channel.getUsers().end(); it++)
    {
        if (it->first != joiner.getSocket())
            sendSuccessMessage(it->first, JOIN(joiner.getNickname(), chaname), "");
    }    
}

void Server::part(ServerMessage serverMessage)
{
    User&   parter = getUserBySocket(serverMessage.getSocket());
    
    // uncomment for evaluation
    //if (parter.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);
 
    std::vector<std::string> channelsLeave = split(serverMessage.getParams()[0], ",");

    std::string reason;
    if (serverMessage.getParams().size() > 1)
        reason = serverMessage.getParams()[1];
    else
        reason = "<Default Reason"; //TODO: better default reason?

    for (std::vector<std::string>::iterator it = channelsLeave.begin(); it != channelsLeave.end(); it++ )
    {
        if (! channelExists(*it))
            throw std::string(ERR_NOSUCHCHANNEL(*it));
        else if (! _channels.find(*it)->second.isUserInChannel(parter))
            throw std::string(ERR_NOTONCHANNEL(*it));
        else
        {
            sendSuccessMessage(parter.getSocket(), PART(parter.getNickname(), *it, reason), "");
            userRmFromChannel(parter, *it);
        }
    }
}

void Server::topic(ServerMessage serverMessage)
{
    User&   user = getUserBySocket(serverMessage.getSocket());
    const std::string chaname = serverMessage.getParams()[0];
    Channel& channel = _channels.find(chaname)->second;

    if (serverMessage.getParams().size() < 1)
        throw std::string(ERR_NEEDMOREPARAMS("TOPIC"));

    if (! channelExists(chaname))
        throw std::string(ERR_NOSUCHCHANNEL(chaname));
    
    if (! channel.isUserInChannel(user))
        throw std::string(ERR_NOTONCHANNEL(chaname));

    // Privilege checking
    if (channel.isTopicRestrict() == true)
    {
        if (channel.isOperator(user) == false)
            throw std::string(ERR_CHANOPRIVSNEEDED(user.getNickname(), chaname));
    }

    if (serverMessage.getParams().size() == 1)
    {
        const std::string&    topic = _channels.find(chaname)->second.getTopic();
        if (topic == "")
            sendSuccessMessage(user.getSocket(), RPL_NOTOPIC(user.getNickname(), chaname), "");
        else
            sendSuccessMessage(user.getSocket(), RPL_TOPIC(user.getNickname(), chaname, topic), "");
    }
    else
    {
        std::string newtopic = "";
        for (size_t i = 1; i < serverMessage.getParams().size(); i++)
            newtopic += serverMessage.getParams()[i] + " ";
        newtopic.erase(0,1); // removing the ":"
        
        channel.setTopic(newtopic);

        //Broadcast change of topic to all users in channel
        if (newtopic == "")
            channel.broadcast(RPL_NOTOPIC(user.getNickname(), chaname));
        else
            channel.broadcast(RPL_TOPIC(user.getNickname(), chaname, newtopic));
    }
}

void Server::names(ServerMessage serverMessage)
{
    User&   user = getUserBySocket(serverMessage.getSocket());
    const std::string chaname = serverMessage.getParams()[0];
    Channel& channel = _channels.find(chaname)->second;

    // uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);

    if (serverMessage.getParams().size() < 1)
        throw std::string(ERR_NEEDMOREPARAMS("NAMES"));

    if (serverMessage.getParams().size() && serverMessage.getParams()[0] == "IRC")
        return;

    std::vector<std::string> chanames = split(serverMessage.getParams()[0], ",");
    if (chanames.size() == 0)
        throw std::string(ERR_NOSUCHNICKCHAN(user.getNickname(), serverMessage.getParams()[0]));
    if (chanames.size() > 1)
        throw std::string(ERR_TOOMANYTARGETS(user.getNickname(), "NAMES"));
    
    const std::string chaname = chanames[0];
    if (! channelExists(chaname))
        throw std::string(ERR_NOSUCHCHANNEL(chaname));
    
    if (_channels.find(chaname)->second.isUserInChannel(user) == false)
        throw std::string(ERR_NOTONCHANNEL(chaname));
    
    Channel& channel = _channels.find(chaname)->second;
    sendSuccessMessage(user.getSocket(), RPL_NAMREPLY(user.getNickname(), chaname, channel.getUsersString()), "");
    sendSuccessMessage(user.getSocket(), RPL_ENDOFNAMES(user.getNickname(), chaname), "");
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
        channel.addInvited(invitedUser);
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
    if (channel.isInvited(kickedUser))
        channel.removeInvited(kickedUser);
    if (channel.getUsers().size() == 0)
        _channels.erase(chaname);
    sendSuccessMessage(user.getSocket(), KICK(user.getNickname(), chaname, kickedUser.getNickname(), reason), "");
    sendSuccessMessage(kickedUser.getSocket(), KICK(user.getNickname(), chaname, kickedName, reason), "");
    channel.broadcastMessagetoChannel(KICK(user.getNickname(), chaname, kickedUser.getNickname(), reason), user);
}

void Server::who(ServerMessage serverMessage)
{
    // uncomment for evaluation
    //if (user.isLoggedIn() == false)
    //    throw std::string(ERR_NOTREGISTERED);
    
    if (serverMessage.getParams().size() != 1)
        throw std::string(ERR_NEEDMOREPARAMS("WHO"));
    
    User&   user = getUserBySocket(serverMessage.getSocket());
    Channel& channel = _channels.find(serverMessage.getParams()[0])->second;
    
    if (!channelExists(serverMessage.getParams()[0]))
        throw std::string(ERR_NOSUCHCHANNEL(serverMessage.getParams()[0]));
    if (channel.isUserInChannel(user) == false)
        throw std::string(ERR_NOTONCHANNEL(serverMessage.getParams()[0]));

    std::map<int, User*>::iterator it = channel.getUsers().begin();
    for(; it != channel.getUsers().end(); it++)
    {
        User&   currUser = *it->second;
        std::string flags = " H";
        if (currUser.isOperator()) // operator in server
            flags += "*";
        if (channel.isOperator(currUser)) // operator in channel
            flags += "@";
        sendSuccessMessage(user.getSocket(), RPL_WHOREPLY(currUser.getNickname(), channel.getName(), currUser.getUsername(), flags), "");
    }
    sendSuccessMessage(user.getSocket(), RPL_ENDOFWHO(user.getNickname(), channel.getName()), "");
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
            channel.broadcastMessagetoChannel(OP_MODE(user.getNickname(), chaname, modes), user);
        }
        else if (mode == 't')
        {
            channel.setTopicRestrict(sign);
            sendSuccessMessage(user.getSocket(), OP_MODE(user.getNickname(), chaname, modes), "");
            channel.broadcastMessagetoChannel(OP_MODE(user.getNickname(), chaname, modes), user);
        }
        else if (mode == 'l' && sign == false)
        {
            channel.setMaxUsers(sign, 0);
            sendSuccessMessage(user.getSocket(), OP_MODE(user.getNickname(), chaname, modes), "");
            channel.broadcastMessagetoChannel(OP_MODE(user.getNickname(), chaname, modes), user);
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
            channel.broadcastMessagetoChannel(OP_MODE(user.getNickname(), chaname, modes + " " + param), user);
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
            channel.broadcastMessagetoChannel(OP_MODE(user.getNickname(), chaname, modes + " " + param), user);
        }
        else if (mode == 'o')
        {
            if (sign)
                channel.addOperator(channel.getUserByNickname(param));
            else 
                channel.removeOperator(channel.getUserByNickname(param));
            sendSuccessMessage(user.getSocket(), OP_MODE(user.getNickname(), chaname, modes + " " + param), "");
            channel.broadcastMessagetoChannel(OP_MODE(user.getNickname(), chaname, modes + " " + param), user);
        }
    }
}
