#pragma once

/********************************/
/*		Colors					*/
/********************************/

# define RED_CMD "\033[31m"
# define NOCOLOR_CMD "\033[0m"

/********************************/
/*		Messages Errors			*/
/********************************/

# define ERROR_ARGV_1 "\033[31mError\nWrong number of Arguments\nUsage: ./ircserv <port> <password>\033[0m\n"
# define ERROR_ARGV_2 "\033[31mError\nPort must be a positive number between 1024 and 65535\033[0m\n"
# define ERROR_ARGV_3 "\033[31mError\nPort must be between 1024 and 65535\033[0m\n"


