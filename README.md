# IRC

Project done by:
- [dadoming](https://github.com/dadoming)
- [Grubben](https://github.com/Grubben)

## Table of Contents
- [Description](#description)
- [Installation](#installation)
- [Usage](#usage)
- [Commands](#commands)

## Description
This project is about creating an Internet Relay Chat (IRC) server. We learned the basic infrastructure of a client-server application, and how to implement it using the TCP protocol.
To do this project we had to learn how sockets work, how event driven programming works, and how to use the select function with non-blocking file descriptors. We also had to learn how numeric replies work, and how a client interprets them.

Here is an image from IBM docs that best represents how our server works:
![alt text](https://www.ibm.com/docs/en/ssw_ibm_i_71/rzab6/rzab6508.gif) 

Resuming the image:
1. The server creates a socket and binds it to a port.
2. The server sets the socket to non-blocking mode.
3. The server listens for connections on the socket.
4. The server creates a list of file descriptors to read from.
5. The server adds the socket to the list.
6. The server has select() on a loop, and waits for activity on the file descriptors.
7. If a client connects, the server accepts the connection and adds the new file descriptor to the list.
8. If a client sends a message, the server reads the message and sends a reply.
9. If a client disconnects, the server removes the file descriptor from the list.
10. If the server receives a command to shut down, it closes all the file descriptors and exits.

FD_SET(), FD_CLR(), etc. are macros used to manipulate the list of file descriptors that select reads from. We add new connections to the list with FD_SET(), and remove them with FD_CLR(). FD_ISSET() is used to check if a file descriptor is in the list.

This is basic server functionality that we had to do. The rest of the project was interaction with clients. For example, when a client connects, it sends a message to a server trying to negotiate capabilities. When capability negotiation is over, the client procceeds to send a NICK and a USER message. The client has been added to the database but not authenticated. The client can only use the server's commands upon entering the PASS command. Then the client receives a 001 numeric reply, which means that the client has been authenticated. The client can now use the server's commands.

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

