#include "Bot.hpp"

void    Bot::ponger(int socket, std::vector<std::string>& msgwords)
{
    std::string pong("PRIVMSG #mustard PONG\r\n");
    sendAll(socket, pong);
}

void    tictactoeSend(int socket, std::map<std::string, std::map<std::string, int> >&   ttt)
{
    for (std::map<std::string, std::map<std::string, int> >::iterator it = ttt.begin(); it != ttt.end(); it++)
    {
        std::string board = "";
        for (std::map<std::string, int>::iterator minit = it->second.begin(); minit != it->second.end(); minit++)
        {
            if (minit->second == 0)
                board += "-";
            else if (minit->second == 1)
                board += "X";
            else
                board += "Y";
        }
        sendAll(socket,  "PRIVMSG #mustard " + board + "\r\n");
    }
}


void    Bot::tictactoe(int socket, std::vector<std::string>& msgwords)
{
    static std::map<std::string, std::map<std::string, int> >   ttt;

    if (msgwords[3] == "start")
    {
        ttt["a"]["1"] = 0;
        ttt["a"]["2"] = 0;
        ttt["a"]["3"] = 0;
        ttt["b"]["1"] = 0;
        ttt["b"]["2"] = 0;
        ttt["b"]["3"] = 0;
        ttt["c"]["1"] = 0;
        ttt["c"]["2"] = 0;
        ttt["c"]["3"] = 0;
        sendAll(socket, "PRIVMSG #mustard Board is ready. Insert commands like: \"! tictactoe a 1\"\r\n");
        return ;
    }

    ttt[msgwords[3]][msgwords[4]] = 1;

    for (std::map<std::string, std::map<std::string, int> >::iterator it = ttt.begin(); it != ttt.end(); it++)
    {
        for (std::map<std::string, int>::iterator minit = it->second.begin(); minit != it->second.end(); minit++)
        {
            if (minit->second == 0)
            {
                ttt[it->first][minit->first] = 2;
                minit->second = 2;
                // std::string aiplay = "PRIVMSG #mustard tictactoe reply: " + it->first + " " + minit->first + "\r\n";
                // sendAll(socket, aiplay);
                tictactoeSend(socket, ttt);
                return ;
            }
        }
    }
    // sendAll(socket, "PRIVMSG #mustard You have won\r\n");
}


Bot::Bot()
    : _summonWord("!")
{
    _botNickname = BOTNAME;
    _botUsername = BOTNAME;

    _socket = socket(AF_INET, SOCK_STREAM, getprotobyname("TCP")->p_proto);
    if (_socket < 0)
    {
        std::cerr << "socket() failed" << std::endl;
        exit(1);
    }
    
    _host = gethostbyname(SERVER_HOSTNAME);
    if (_host == NULL)
    {
        std::cerr << "gethostbyname() failed" << std::endl;
        exit(1);
    }

    memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(BOT_PORT);
    memcpy(&_addr.sin_addr, _host->h_addr, _host->h_length);

    if (connect(_socket, (struct sockaddr*)&_addr, sizeof(_addr)) < 0)
    {
        std::cerr << "connect() failed" << std::endl;
        exit(1);
    }

    _actions["PING"] = ponger;
    _actions["tictactoe"] = tictactoe;
}

int main(void)
{
    Bot bot;

    std::string nick("NICK " + bot.getBotNickname() + "\r\n");
    sendAll(bot.getSocket(), nick);

    std::string user("USER " + bot.getBotUsername() + " 0 * :" + bot.getBotNickname() + "\r\n");
    sendAll(bot.getSocket(), user);

    char buffer[1024];

    int len = recv(bot.getSocket(), &buffer, sizeof(buffer) - 1, 0);
    buffer[len] = 0;
    std::cout << buffer << std::endl;

    std::string pass("PASS " + std::string(CONNECTION_PASSWORD) + "\r\n");
    sendAll(bot.getSocket(), pass);
    
    len = recv(bot.getSocket(), &buffer, sizeof(buffer) - 1, 0);
    buffer[len] = 0;
    std::cout << buffer << std::endl;

    std::string joinmsg("JOIN #mustard\r\n");
    sendAll(bot.getSocket(), joinmsg);

    int i = 1;
    while (1)
    {
        len = recv(bot.getSocket(), &buffer, sizeof(buffer) - 1, 0);
        buffer[len] = 0; 
        std::string msg = buffer;
        std::cout << msg << std::endl;

        std::vector<std::string> first = split(trim(msg), "#");

        if (i)
        {
            i--;
            continue;
        }
        if (first.size() > 1)
        {
            msg = "#" + first[1];
            std::cout << msg << std::endl;

            std::vector<std::string>    msgwords = split(msg, " ");
            if (msgwords.size() < 2)
                continue;
            std::cout << msgwords[1] << std::endl;
            msgwords[1].erase(0,1);


            const std::string& chaname = msgwords[0];

            std::cout << msgwords[0] << std::endl;
            std::cout << msgwords[1] << std::endl;

            if (msgwords[1] == bot._summonWord)
            {
                // if (msgwords[2] == "PING" )
                std::cout << "what if NULL?: " << bot._actions["ASDASDASD"] << std::endl;
                if (bot._actions[msgwords[2]] != NULL)
                {
                    bot._actions[msgwords[2]](bot.getSocket(), msgwords);
                }
            }
        }
    }

    return 0;
}