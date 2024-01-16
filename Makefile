NAME = ircserv

CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98 -D DEBUG=$(DEBUG)
RM = rm -rf

Y = "\033[33m"
R = "\033[31m"
G = "\033[32m"
B = "\033[34m"
X = "\033[0m"

SRC_DIR = src/
OBJ_DIR = obj/

CPPFILES = 	main.cpp \
			Server/Server.cpp \
			Utils/checkArgs.cpp \

SRCS = ${addprefix $(SRC_DIR), $(CPPFILES)}
OBJECTS = ${patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.opp, $(SRCS)}

HEADERS = 	includes/Server.hpp \
			includes/Messages.hpp \
			includes/Utils.hpp \

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

all: ${NAME}

${NAME}: $(HEADERS) ${OBJECTS}
	@echo $(G)Created Objects folder$(X)
	@echo $(G)Finished Compiling of [$(CPPFILES)]$(X)
	@echo
	@echo $(Y)Compiling [$(NAME)]...$(X)
	@${CC} ${CFLAGS} $(INCLUDES) ${OBJECTS} -o ${NAME}
	@echo $(G)Finished Compiling of [$(NAME)]$(X)

$(OBJ_DIR)%.opp: $(SRC_DIR)%.cpp
	@mkdir -p $(shell dirname $@)
	@echo $(Y)Compiling [$<]...$(X)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

clean:
	@$(RM) $(OBJ_DIR)
	@echo $(R)Removed objects folder$(X)

fclean: clean
	@${RM} ${NAME}
	@echo $(R)Removed following executable: [$(NAME)]$(X)

re: fclean all

debug:
	@echo $(B)Debug mode$(X)
	make re DEBUG=1

leaks: ${NAME}
	@echo $(B)Checking for leaks...$(X)
	${LEAKS}

val: ${NAME}
	@echo $(B)Checking with valgrind...$(X)
	@valgrind ./${NAME} ${ARG}

.PHONY:	all clean fclean re debug leaks val
