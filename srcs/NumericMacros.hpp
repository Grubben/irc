#ifndef NUMERICMACROS_HPP_
#define NUMERICMACROS_HPP_

#include <iostream>
#include "Server.hpp"

// 


#define ERR_NOSUCHNICK(nickname)                    (":" SERVER_NAME " 401 " + nickname + " : Username or Nickname invalid\r\n")
#define ERR_NOSUCHNICKCHAN(sender, receiver)        (":" SERVER_NAME " 401 " + sender + " " + receiver + " :Unexistent Nickname/Channel name\r\n")
#define ERR_NOSUCHCHANNEL(channel)                  (":" SERVER_NAME " 403 " + channel + " :No such channel\r\n")
#define ERR_NONICKNAMEGIVEN                         (":" SERVER_NAME " 431 :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(nickname, reason)      (":" SERVER_NAME " 432 " + nickname + " :" + reason + "\r\n")
#define ERR_NICKNAMEINUSE(nickname)                 (":" SERVER_NAME " 433 " + nickname + " :Nickname is already in use\r\n")
#define ERR_NOTONCHANNEL(channel)                   (":" SERVER_NAME " 442 " + channel + " :You're not on that channel\r\n")
#define ERR_NEEDMOREPARAMS(command)                 (":" SERVER_NAME " 461 " command " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED                        (":" SERVER_NAME " 462 : Already Registered\r\n")
#define ERR_PASSWDMISMATCH                          (":" SERVER_NAME " 464 : Password incorrect\r\n")
#define ERR_USERONCHANNEL(nickname, channel)        (":" SERVER_NAME " 443 " + nickname + " " + channel + " :is already on channel\r\n")


#define RPL_WELCOME(nickname)                       (":" SERVER_NAME " 001 " + nickname + " :Welcome to the <inserir nome> IRC Network, " + nickname + "\r\n")
#define RPL_YOUREOPER                               (":" SERVER_NAME " 381 : You are now an IRC operator\r\n")
#define RPL_TOPIC(nickname, channel, topic)         (":" SERVER_NAME " 332 " + nickname + " " + channel + " :" + topic + "\r\n")
#define RPL_NOTOPIC(nickname, channel)              (":" SERVER_NAME " 331 " + nickname + " " + channel + " :No topic is set\r\n")
// If we do the bonus, implement this: https://ircv3.net/specs/extensions/bot-mode
#define RPL_WHOISBOT(nickname, bots_name)           (":" SERVER_NAME " 335 " + nickname + " " + bots_name + " :is a bot\r\n")
#define RPL_INVITING(inviter, invitee, channel)     (":" SERVER_NAME " 341 " + inviter + " " + invitee + " " + channel + " :Inviting you to channel\r\n")
#define RPL_NAMREPLY(nickname, channel, nicknames)  (":" SERVER_NAME " 353 " + nickname + " = " + channel + " :" + nicknames + "\r\n")
#define RPL_ENDOFNAMES(nickname, channel)           (":" SERVER_NAME " 366 " + nickname + " " + channel + " :End of NAMES list\r\n")


#define JOIN(joiner, channel)                       (":" + joiner + "!" + joiner + "@" + SERVER_NAME " JOIN " + channel + "\r\n")
#define QUIT(nickname)                              (":" + nickname + "!" + nickname + "@" + SERVER_NAME + " QUIT :Bye bye\r\n")
#define PRIVMSG(sender, receiver, message)          (":" + sender + "!" + sender + "@" + SERVER_NAME + " PRIVMSG " + receiver + " :" + message + "\r\n")
#define PART(nickname, channel, reason)             (":" + nickname + "!" + nickname + "@" + SERVER_NAME + " PART " + channel + " :" + reason + "\r\n")
#define INVITE(inviter, invitee, channel)           (":" + inviter + "!" + inviter + "@" + SERVER_NAME + " INVITE " + invitee + " " + channel + "\r\n")

#endif