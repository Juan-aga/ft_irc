#pragma once

/********************************/
/*		Colors					*/
/********************************/

# define RED_CMD "\033[31m"
# define GREEN_CMD "\033[32m"
# define YELLOW_CMD "\033[33m"
# define BLUE_CMD "\033[34m"
# define NOCOLOR_CMD "\033[0m"

/********************************/
/*		Messages Errors			*/
/********************************/

# define ERROR_ARGV_1 "\033[31mError\nWrong number of Arguments\nUsage: ./ircserv <port> <password>\033[0m\n"
# define ERROR_ARGV_2 "\033[31mError\nPort must be a positive number between 1024 and 65535\033[0m\n"
# define ERROR_ARGV_3 "\033[31mError\nPort must be between 1024 and 65535\033[0m\n"

/********************************/
/*		Messages Responses		*/
/********************************/

/*NICK*/
# define NO_NICK_GIVE	"No nickname given"
# define ERRONEUS_NICK	"Erroneus nickname"
# define IN_USE_NICK	"Nickname is already in use"

/*USER*/
# define MORE_PARAMS	"Not enough parameters"
# define NOR_REGSTERED	"You may not reregister"
