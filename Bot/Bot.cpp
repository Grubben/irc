#include "Bot.hpp"

Bot::Bot()
    : _summonWord("/bot")
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

    std::string joinmsg("JOIN #BotChannel\r\n");
    sendAll(bot.getSocket(), joinmsg);

    while (1)
    {
        len = recv(bot.getSocket(), &buffer, sizeof(buffer) - 1, 0);
        buffer[len] = 0;
        std::cout << buffer << std::endl;
    
        if (strstr(buffer, "PING") != NULL)
        {
            std::string pong("PRIVMSG #BotChannel PONG\r\n");
            sendAll(bot.getSocket(), pong);
        }
    }

    return 0;
}