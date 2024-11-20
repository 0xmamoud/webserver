NAME = webserv

SRC_DIR = src
CORE_DIR = src/core
CGI_DIR = src/cgi
CONFIG_DIR = src/config
HTTP_DIR = src/http
LOG_DIR = src/log
UTILS_DIR = src/utils
OBJ_DIR = obj

HEADER = include

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
INCLUDES = -I $(HEADER)
RM = rm -rf

SRC = ${SRC_DIR}/main.cpp \
	${CONFIG_DIR}/ConfigParser.cpp \
	${CORE_DIR}/Server.cpp \
	${CORE_DIR}/Connection.cpp \
	${CORE_DIR}/Epoll.cpp \
	${LOG_DIR}/Logger.cpp \

OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRC)))

vpath %.cpp $(SRC_DIR) $(CORE_DIR) $(CGI_DIR) $(CONFIG_DIR) $(HTTP_DIR) $(UTILS_DIR) $(LOG_DIR)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) -o $(NAME)
	@echo "\033[32mwebserv compiled\033[0m"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@$(RM) $(OBJ_DIR)
	@echo "\033[31mwebserv object files removed\033[0m"

fclean: clean
	@$(RM) $(NAME)
	@echo "\033[31mwebserv removed\033[0m"

re: fclean all

.PHONY: all clean fclean re
