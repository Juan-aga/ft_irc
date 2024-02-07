NAME = ircserv
# NAME_BONUS = ircserv_bonus

HOST = $(shell hostname)

CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98 -g -D DEBUG=$(DEBUG) -D HOST=\"$(HOST)\"
RM = rm -rf

Y = \033[33m
R = \033[31m
G = \033[32m
B = \033[34m
X = \033[0m

SRC_DIR = src/
OBJ_DIR = obj/

# SRC_DIR_BONUS = bonus/src/
# OBJ_DIR_BONUS = bonus/obj/

CPPFILES = 	main.cpp 						\
			Server/Server.cpp 				\
			Utils/checkArgs.cpp 			\
			Utils/addFileLog.cpp			\
			Utils/getTime.cpp				\
			Utils/signals.cpp				\
			Commands/Commands.cpp			\
			Commands/connectionCommands.cpp	\
			Commands/channelCommands.cpp	\
			Client/Client.cpp				\
			Res/Response.cpp				\
			Channel.cpp

# CPPFILES_BONUS = 	main_bonus.cpp 							\
# 					Server/Server_bonus.cpp 				\
# 					Utils/checkArgs_bonus.cpp 				\
# 					Utils/addFileLog_bonus.cpp				\
# 					Utils/getTime_bonus.cpp					\
# 					Utils/signals_bonus.cpp					\
# 					Commands/Commands_bonus.cpp				\
# 					Commands/connectionCommands_bonus.cpp	\
# 					Commands/channelCommands_bonus.cpp		\
# 					Client/Client_bonus.cpp					\
# 					Res/Response_bonus.cpp					\
# 					Channel_bonus.cpp

SRCS = ${addprefix $(SRC_DIR), $(CPPFILES)}
OBJECTS = ${patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.opp, $(SRCS)}

# SRCS_BONUS = ${addprefix $(SRC_DIR_BONUS), $(CPPFILES_BONUS)}
# OBJECTS_BONUS = ${patsubst $(SRC_DIR_BONUS)%.cpp, $(OBJ_DIR_BONUS)%.opp, $(SRCS_BONUS)}

HEADERS = 	includes/Server.hpp 	\
			includes/Messages.hpp	\
			includes/Utils.hpp		\
			includes/Commands.hpp	\
			includes/Client.hpp		\
			includes/Response.hpp	\
			includes/Channel.hpp

# HEADERS_BONUS = 	includes/Server_bonus.hpp 		\
# 					includes/Messages_bonus.hpp		\
# 					includes/Utils_bonus.hpp		\
# 					includes/Commands_bonus.hpp		\
# 					includes/Client_bonus.hpp		\
# 					includes/Response_bonus.hpp		\
# 					includes/Channel_bonus.hpp

INCLUDES =	-I ./includes

# INCLUDES_BONUS = -I ./bonus/includes

ifdef DEBUG
	CFLAGS += -g
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
	@${RM} ircserv.log
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
	@valgrind --leak-check=full --show-leak-kinds=all ./${NAME} ${ARG}

# bonus: credit ${NAME_BONUS}

# ${NAME_BONUS}: head $(HEADERS_BONUS) ${OBJECTS_BONUS}
# 	@${CC} ${CFLAGS} $(INCLUDES) $(INCLUDES_BONUS) ${OBJECTS_BONUS} -o ${NAME_BONUS}
# 	@echo "$(Y)█████████████████████ Compiling is DONE ███████████████████████$(X)"

# $(OBJ_DIR_BONUS)%.opp: $(SRC_DIR_BONUS)%.cpp
# 	@mkdir -p $(shell dirname $@)
# 	@printf "$(Y)█Compiling$(X) $<:\r\t\t\t\t\t\t\t..."
# 	@$(CC) $(CFLAGS) $(INCLUDES) $(INCLUDES_BONUS) -c $^ -o $@
# 	@echo "\r\t\t\t\t\t\t\t$(G){DONE}$(Y) █$(X)"

# clean_bonus:
# 	@$(RM) $(OBJ_DIR_BONUS)
# 	@echo "$(R)Removed objects folder$(X)"

# fclean_bonus: clean_bonus
# 	@${RM} ${NAME_BONUS}
# 	@${RM} ircserv_bonus.log
# 	@echo "$(R)Removed following executable: [$(NAME_BONUS)]$(X)"

# re_bonus: fclean_bonus bonus

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

.PHONY:	all clean fclean re debug leaks val bonus credit lol head clean_bonus fclean_bonus re_bonus
