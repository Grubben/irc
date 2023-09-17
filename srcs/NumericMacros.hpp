#ifndef NUMERICMACROS_HPP_
#define NUMERICMACROS_HPP_

#include <iostream>
#include "Server.hpp"

#define ERR_NONICKNAMEGIVEN                     std::string(":" SERVER_NAME " 431 :No nickname given\r\n")
#define ERR_NICKNAMEINUSE(nickname)             std::string(":" SERVER_NAME " 433 " + nickname + " :Nickname is already in use\r\n")
#define ERR_ERRONEUSNICKNAME(nickname, reason)  std::string(":" SERVER_NAME " 432 " + nickname + " :" + reason + "\r\n")
#define ERR_NEEDMOREPARAMS(command)             std::string(":" SERVER_NAME " 461 " command " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED                    std::string(":" SERVER_NAME " 462 :Already Registered\r\n")
#define ERR_PASSWDMISMATCH                      std::string(":" SERVER_NAME " 464 :Password incorrect\r\n")
#define ERR_NOSUCHNICK(nickname)                std::string(":" SERVER_NAME " 401 " + nickname + " :No such nickname\r\n")
#define ERR_NOSUCHNICKCHAN(sender, receiver)    std::string(":" SERVER_NAME " 401 " + sender + " " + receiver + " :Unexistent Nickname/Channel name\r\n")

#define RPL_WELCOME(nickname)                   std::string(":" SERVER_NAME " 001 " + nickname + " :Welcome to the <inserir nome> IRC Network, " + nickname + "\r\n")
#define RPL_YOUREOPER                           std::string(":" SERVER_NAME " 381 :You are now an IRC operator\r\n")

// If we do the bonus, implement this: https://ircv3.net/specs/extensions/bot-mode
#define RPL_WHOISBOT(nickname, bots_name)       std::string(":" SERVER_NAME " 335 " + nickname + " " + bots_name + " :is a bot\r\n")

#define PRIVMSG(sender, receiver, message)      std::string(":" + sender + "!" + sender + "@" + SERVER_NAME + " PRIVMSG " + receiver + " :" + message + "\r\n")


#endif