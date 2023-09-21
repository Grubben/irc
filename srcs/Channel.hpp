#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once
#include <iostream>
#include <list>
#include "Exceptions.hpp"
#include "Server.hpp"

class Channel
{
private:
	Server&					_server;
	const std::string		_name;
	std::string				_topic;

	std::map<int, User*>	_chanusers;

	// Invite-Only     -i
	bool						_isInviteOnly;
	std::vector<std::string>	_invited;
	// Topic-Restrict  -t
	bool 						_topicRestrict;
	// Key             -k
	bool 						_hasPassword;
	std::string 				_password;
	// User Limit      -l
	bool 						_userLimit;
	int 						_maxUsers; // if 0 no limit
	// Operators       -o
	std::vector<User*>			_operators;

	void	usersDrop(); //Not API: only used internally
public:
	Channel(Server& server, std::string name);
	Channel(const Channel& copy);
	Channel& operator=(const Channel& copy);
	~Channel();

	/*	Getters & Setters	*/
	const std::string&		getName(void) const { return (_name); };
	std::map<int,User*>&	getUsers(void) { return _chanusers; };
	const std::string&		getPassword(void) const { return _password; };
	int 				getMaxUsers(void) const { return _maxUsers; };
	User&               getUserByNickname(std::string nickname) { for (std::map<int,User*>::iterator it = _chanusers.begin(); it != _chanusers.end(); it++) { if (it->second->getNickname() == nickname) return *(it->second); } throw ChannelUnableToFindUser(); };
	std::string				getUsersString(void) { std::string users = ""; for (std::map<int,User*>::iterator it = _chanusers.begin(); it != _chanusers.end(); it++) { std::string tmp = it->second->getNickname(); users += tmp + " "; } return users; };
	bool 				isInviteOnly(void) const { return _isInviteOnly; };
	bool 				isInvited(std::string username) { for (std::vector<std::string>::iterator it = _invited.begin(); it != _invited.end(); it++) { if (*it == username) return true; } return false; };
	bool 				isTopicRestrict(void) const { return _topicRestrict; };
	bool				isOperator(User& user) const { for (std::vector<User*>::const_iterator it = _operators.begin(); it != _operators.end(); it++) { if ((*it)->getSocket() == user.getSocket()) return true; } return false; };
	bool 				hasPassword(void) const { return _hasPassword; };
	bool 				hasUserLimit(void) const { return _userLimit; };
	void 					addOperator(User& user) { if (!isOperator(user)) _operators.push_back(&user); };
	void 					addInvited(std::string username) { if (!isInvited(username)) _invited.push_back(username); };
	void 					removeOperator(User& user) { for (std::vector<User*>::iterator it = _operators.begin(); it != _operators.end(); it++) { if ((*it)->getSocket() == user.getSocket()) { _operators.erase(it); return ; } } };
	void 					removeInvited(std::string username) { for (std::vector<std::string>::iterator it = _invited.begin(); it != _invited.end(); it++) { if (*it == username) { _invited.erase(it); return ; } } };
	void 					setPassword(bool sign, std::string pass) { if (sign == false && pass == _password){_password = ""; _hasPassword = false; }else{_password = pass; _hasPassword = true;} };
	void 					setMaxUsers(bool sign, int max) { if (max == 0 || sign == false){ _maxUsers = 0; _userLimit = false; }else{_maxUsers = max; _userLimit = true;} }
	void 					setInviteOnly(bool inviteOnly) { _isInviteOnly = inviteOnly; };
	void 					setTopicRestrict(bool topicRestrict) { _topicRestrict = topicRestrict; };
	// void 				setName(std::string name) { _name = name; } //Warning: should delete. Name cannot be changed
	const std::string&	getTopic() const { return _topic; };
	void				setTopic(const std::string& newtopic) { _topic = newtopic; };

	std::string getModes(void) {
		std::stringstream modeString;
        modeString << "+";
        if (_isInviteOnly)
            modeString << "i";
        if (_topicRestrict)
            modeString << "t";
        if (_hasPassword)
            modeString << "k";
        if (_userLimit)
            modeString << "l";
        
        if (_userLimit)
            modeString << " " << _maxUsers;
        if (_hasPassword)
            modeString << " " << _password;
        
        std::string tmp = modeString.str();
        std::cout << tmp << std::endl;
		if (tmp.length() == 1)
            tmp = "";
		
		return tmp;
	}

	/*	API	*/
	void	userAdd(User& user);
	int		userRemove(User& user);
	bool	isUserInChannel(User& user);
	void	broadcast(std::string msg);
};


#endif