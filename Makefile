NAME = ircserv

HOST = $(shell hostname)

CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98 -D DEBUG=$(DEBUG) -D HOST=\"$(HOST)\"
RM = rm -rf

Y = \033[33m
R = \033[31m
G = \033[32m
B = \033[34m
X = \033[0m

SRC_DIR = src/
OBJ_DIR = obj/

CPPFILES = 	main.cpp 				\
			Server/Server.cpp 		\
			Utils/checkArgs.cpp 	\
			Commands/Commands.cpp	\
			Client/Client.cpp

SRCS = ${addprefix $(SRC_DIR), $(CPPFILES)}
OBJECTS = ${patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.opp, $(SRCS)}

HEADERS = 	includes/Server.hpp 	\
			includes/Messages.hpp	\
			includes/Utils.hpp		\
			includes/Commands.hpp	\
			includes/Client.hpp

INCLUDES =	-I ./includes

ifdef DEBUG
	DEBUG	=	1
else
	DEBUG	=	0
endif

UNAME_S		:=	$(shell uname -s)

ifeq ($(UNAME_S), Darwin)
		LEAKS =	@leaks -q -atExit -- ./${NAME} ${ARG}
else
		LEAKS =	@echo "Can't use leaks in" ${UNAME_S}.
endif

ifndef	ARG
		ARG	=	
endif

all: credit ${NAME}

${NAME}: head $(HEADERS) ${OBJECTS}
	@${CC} ${CFLAGS} $(INCLUDES) ${OBJECTS} -o ${NAME}
	@echo "$(Y)██████████████████████ Compiling is DONE ███████████████████████$(X)"

head:
	@echo "$(Y)█████████████████████ Making ft_irc Server █████████████████████$(X)"

$(OBJ_DIR)%.opp: $(SRC_DIR)%.cpp
	@mkdir -p $(shell dirname $@)
	@printf "$(Y)█Compiling$(X) $<:\r\t\t\t\t\t\t\t..."
	@$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@
	@echo "\r\t\t\t\t\t\t\t$(G){DONE}$(Y) █$(X)"

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(R)Removed objects folder$(X)"

fclean: clean
	@${RM} ${NAME}
	@echo "$(R)Removed following executable: [$(NAME)]$(X)"

re: fclean all

debug:
	@echo "$(B)Debug mode$(X)"
	make re DEBUG=1

leaks: ${NAME}
	@echo "$(B)Checking for leaks...$(X)"
	${LEAKS}

val: ${NAME}
	@echo "$(B)Checking with valgrind...$(X)"
	@valgrind ./${NAME} ${ARG}

credit:
	@echo " ┏━━━┓┏━━━━┓    ┏━━┓┏━━━┓┏━━━┓"
	@echo " ┃┏━━┛┃┏┓┏┓┃    ┗┫┣┛┃┏━┓┃┃┏━┓┃"
	@echo " ┃┗━━┓┗┛┃┃┗┛     ┃┃ ┃┗━┛┃┃┃ ┗┛"
	@echo " ┃┏━━┛  ┃┃       ┃┃ ┃┏┓┏┛┃┃ ┏┓"
	@echo " ┃┃    ┏┛┗┓┏━━━┓┏┫┣┓┃┃┃┗┓┃┗━┛┃"
	@echo " ┗┛    ┗━━┛┗━━━┛┗━━┛┗┛┗━┛┗━━━┛"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "━━━━━━━━━━━━━━━━━┃ $(Y)paescano juan-aga preina-g$(X) ┃━━━━━━━━━━━━━━━━━"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

lol:
	@echo "⠀⠀⠀⠀⢀⣴⣿⣿⣷⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⣠⡿⠟⠛⠉⠉⠙⠿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⣰⠏⠁⠀⠀⠀⠀⠀⠀⠘⢷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠀⢰⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⢠⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣶⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⣏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠙⠒⠲⠦⢤⣄⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠘⢦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠛⠓⠶⠦⣤⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠈⠳⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠛⠒⠶⢦⣄⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠈⠳⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣇⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠈⠙⢶⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⣠⡿⢦⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠹⣷⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⠟⠁⠀⠀⠈⠉⠑⠲⢤⣀⡀⠈⠻⣇⠀⣷⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⢷⡖⠚⠻⣧⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡇⠀⠀⢻⡄⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⡧⠀⠀⠀⣿⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣿⣷⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⠗⠒⠤⣀⠀⠀⠀⠀⠀⠀⢰⠏⠀⠀⠀⢰⡏⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠚⠛⠛⠛⠛⠛⠆⠀⠀⢀⡿⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣄⠙⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡾⠁⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣷⣄⡙⠷⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⠀⠀⠀⢀⡾⠁⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⡛⢿⣶⣤⣉⡉⠉⠉⠉⠉⠉⠙⠛⠿⢿⣟⡛⠛⠛⠚⠋⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢳⡄⠙⢧⡈⠉⠓⠲⣦⣄⡀⠀⠀⠀⠀⠈⠙⠳⢦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣦⠀⠙⣦⡀⠀⠀⠙⢿⣶⣄⡀⠀⠀⠀⠀⠀⠈⠛⠦⣄⡀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢷⣄⠈⠻⣦⡀⠀⠀⠈⠻⣿⡓⠶⢤⣄⡀⠀⠀⠀⠀⠉⠓⠦⣄⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣦⡀⠙⣿⠶⣄⡀⠀⠈⠻⢦⡀⠀⠉⠙⠒⠦⣤⣄⡀⠀⠈⠙⣆"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢷⣄⣾⠀⠈⠛⢦⡀⠀⠀⠙⢦⡀⠀⠀⠀⠀⠈⠙⠓⠦⣤⡼"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠁⠀⠀⠀⠀⠙⢶⣄⠀⠀⠻⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀"
	@echo "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠳⣦⣠⠇"
	@echo "with love by: paescano juan-aga preina-g"

.PHONY:	all clean fclean re debug leaks val

# NAME = ./ircserv

# # Project's directories
# SRCS_FOLDER = srcs
# OBJECTSDIR = objects
# HEADERS_FOLDER = headers

# # Name of variables
# FLAGS = -Wall -Wextra -Werror
# CPP_STANDARD = -std=c++98
# RED = \033[1;31m
# GREEN = \033[1;32m
# YELLOW = \033[1;33m
# BLUE = \033[1;34m
# RESET = \033[0m

# # Exercise files variable
# SRC_FILES = Client.cpp \
# Server.cpp \
# commands.cpp \
# getSocket.cpp \
# messagesHandling.cpp \
# pollHandling.cpp \
# requestHandling.cpp \
# Channel.cpp \
# DeezNuts.cpp \
# utils.cpp \
# PartCommand.cpp \
# JoinCommand.cpp \
# KickCommand.cpp \
# PrivmsgCommand.cpp \
# FileTransfer.cpp \
# Request.cpp \
# File.cpp \

# HEADERS_FILES = headers/Client.hpp \
# headers/Server.hpp \
# headers/Channel.hpp \
# headers/Request.hpp \
# headers/File.hpp \

# MAIN = main.cpp
# # Define objects for all sources
# OBJS := $(addprefix $(OBJECTSDIR)/, $(SRC_FILES:.cpp=.o))
# MAIN_OBJ = $(addprefix $(OBJECTSDIR)/, $(MAIN:.cpp=.o))

# # Name the compiler
# CC = c++

# # OS specific part
# RM = rm -rf
# RMDIR = rm -rf
# MKDIR = mkdir -p
# MAKE = make -C
# ECHO = /bin/echo
# ERRIGNORE = 2>/dev/null

# all: credit $(NAME)
# 	@echo "$(BLUE)██████████████████████ Compiling is DONE ███████████████████████$(RESET)"
# 	@echo "       Made with love by : \033[1;91mmbari asfaihi abdel-ke\033[m"

# head:
# 	@echo "$(BLUE)█████████████████████ Making ft_irc Server █████████████████████$(RESET)"

# # Phonebook making rules
# $(NAME): head $(OBJS) $(MAIN_OBJ) $(HEADERS_FILES)
# 	@$(CC) $(CPP_STANDARD) $(OBJECTSDIR)/main.o $(OBJS) -o $@ -g

# $(OBJECTSDIR)/%.o: $(SRCS_FOLDER)/%.cpp $(HEADERS_FILES)
# 	@$(MKDIR) $(dir $@)
# 	@printf "$(BLUE)█ $(YELLOW)Compiling$(RESET) $<:\r\t\t\t\t\t\t\t..."
# 	@$(CC) $(CPP_STANDARD) $(FLAGS) -o $@ -c $< -g
# 	@echo "\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)"

# $(OBJECTSDIR)/%.o: main.cpp $(HEADERS_FILES)
# 	@printf "$(BLUE)█ $(YELLOW)Compiling$(RESET) $<:\r\t\t\t\t\t\t\t..."
# 	@$(CC) $(CPP_STANDARD) $(FLAGS) -o $@ -c $< -g
# 	@echo "\r\t\t\t\t\t\t\t$(GREEN){DONE}$(BLUE) █$(RESET)"

# # Remove all objects, dependencies and executable files generated during the build
# clean:
# 	@echo "$(RED)deleting$(RESET): " $(OBJECTSDIR)
# 	@$(RMDIR) $(OBJECTSDIR)


# fclean: clean
# 	@echo "$(RED)deleting$(RESET): " $(NAME)
# 	@$(RM) $(NAME) $(ERRIGNORE)

# re: fclean all

# # Print Credit
# credit:
# 	@echo " ┏━━━┓┏━━━━┓    ┏━━┓┏━━━┓┏━━━┓"
# 	@echo " ┃┏━━┛┃┏┓┏┓┃    ┗┫┣┛┃┏━┓┃┃┏━┓┃"
# 	@echo " ┃┗━━┓┗┛┃┃┗┛     ┃┃ ┃┗━┛┃┃┃ ┗┛"
# 	@echo " ┃┏━━┛  ┃┃       ┃┃ ┃┏┓┏┛┃┃ ┏┓"
# 	@echo " ┃┃    ┏┛┗┓┏━━━┓┏┫┣┓┃┃┃┗┓┃┗━┛┃"
# 	@echo " ┗┛    ┗━━┛┗━━━┛┗━━┛┗┛┗━┛┗━━━┛"
# 	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
# 	@echo "━━━━━━━━━┃ Made with love by : \033[1;91mmbari asfaihi abdel-ke\033[m ┃━━━━━━━━━"
# 	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
