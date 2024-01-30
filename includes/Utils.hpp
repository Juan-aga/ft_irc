#pragma once

#ifndef DEBUG
# define DEBUG 0
#endif

/********************************/
/*		Libraries				*/
/********************************/
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <algorithm>
#include <poll.h>
#include <fcntl.h>
#include <ostream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include "Messages.hpp"
// only in linux, to use setw and setfill
// we can change it with (numero < 10 ? "00" : (numero < 100 ? "0" : "")) + std::to_string(numero)
#include <iomanip>
/********************************/
/*		Utility functions		*/
/********************************/

/*	Enum for types of state on the client */

enum	CLIENT_STATUS{
	UNKNOWN,
	AUTH,
	CONNECTED,
	DISCONECT
};

/**
 * @brief check the number of arguments and if 
 * the port is a number and a valid port
 * between 1024 and 65535
 * 
 * @param argc number of arguments
 * @param argv arguments
 */
void checkArgs(int argc, char **argv);
/**
 * @brief Get the actual time
 * 
 * @return std::string with the actual time in format
 * dd/mm/yyyy hh:mm:ss
 */
std::string getTime();
/**
 * @brief Add a message to the ircserv.log file
 * 
 * @param message message to add
 * @param color color of the message
 * @return true if the message was added
 * @return false if the message wasn't added
 */
bool addFileLog(std::string message, std::string color);
/**
 * @brief init the signals for control + c to close the server
 * 
 */
void ft_init_signals(void);