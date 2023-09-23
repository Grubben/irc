# IRC

Project done by:
- [dadoming](https://github.com/dadoming)
- [Grubben](https://github.com/Grubben)

## Table of Contents
- [Introduction](#introduction)
- [Installation](#installation)
- [Usage](#usage)
- [Commands](#commands)

## Introduction
This project is about creating an Internet Relay Chat (IRC) server. We learned the basic infrastructure of a client-server application, and how to implement it using the TCP protocol.
To do this project we had to learn how sockets work, how event driven programming works, and how to use the select function. We also had to learn how numeric replies work, and how a client interprets them.

The client we chose to use is [HexChat](https://hexchat.github.io/), which is a free IRC client for Windows and Unix-like operating systems.

## Installation
Firstly you need to install HexChat, you can download it [here](https://hexchat.github.io/downloads.html).

Then you need to clone the repository:
```bash
git clone https://github.com/Grubben/irc.git
```

Then just cd into the repository and run the Makefile:
```bash
cd irc && make && ./ircserv <port> <password>
```


## Usage

To connect to the server you need to open HexChat and click on the "Add" button and insert a name for your server, then click on "Edit" and insert the server address and port, tick off the "ssl for all servers" and set the login method to SASL (username + password) then click on "Close" and "Connect".

A message saying "This server requires a password should appear" and you need to insert the password you set when you started the server by typping "/pass <password>".

## Commands

Firstly all commands start with a "/" and then the command name, for example "/nick <nickname>".

- /pass [password] - Is used to authenticate you as a user after nickname and username has been provided and accepted. (This is the last step of the registration onto the server.)
- /nick [nickname] - Changes your nickname to the one you want.
- /user [username] [hostname] [servername] [realname] - Is used to set your username, hostname, servername and realname for authentication in case basic registration fails or in case of using terminal clients like nc or telnet.
- /join [channel] [password] - Joins the channel you want.
- /part [channel] [message] - Leaves the channel you want.
- /list - Lists all the channels.
- /names [channel] - Lists all the users in the channel.
- /msg [nickname][,nickname-or-channel] [message] - Sends a private message to all the nicknames or channels you want.
- /quit - Disconnects you from the server.
- /topic [channel] [topic] - Sets the topic of the channel.
- /kick [channel] [nickname] [reason] - Kicks the user from the channel.
- /invite [nickname] [channel] - Invites the user to the channel.
- /oper [username] [server_password] - Authenticates you as an IRC operator.
- /who [channel] - Shows information about all the user in channel.
- /mode [channel] [mode] - Sets the mode of the channel.

About the mode command:

Operators:
- "-" is used to remove a mode.
- "+" is used to add a mode.

Modes:
- Invite-Only (+i) - Only users who have been invited can join the channel.
- Topic-Settable (+t) - Only operators can change the topic of the channel.
- Channel-Operator (+o) - Gives operator status to a user.
- Key (+k) - Sets a password to the channel.
- User-Limit (+l) - Sets a limit of users to the channel.

