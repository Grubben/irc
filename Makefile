NAME = ircserv

CXX = c++

CXXFLAGS = -Wall -Wextra -g -std=c++98 #-Werror

# SRCS = srcs/main.cpp srcs/ServerEnvironment.cpp srcs/Server.cpp srcs/Channel.cpp srcs/User.cpp srcs/Exceptions.cpp
SRCS = $(wildcard srcs/*.cpp)
SRCS_LIST = $(notdir $(SRCS))

OBJS_LIST = $(SRCS_LIST:%.cpp=%.o)
OBJS_DIR = objs/
OBJS = $(addprefix objs/, $(OBJS_LIST))

all: $(NAME)

$(NAME): $(OBJS_DIR) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(OBJS_DIR)%.o: srcs/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

bot: all
	$(CXX) $(CXXFLAGS) -o bot Bot/Bot.cpp srcs/Utils.cpp

.PHONY: all clean fclean re