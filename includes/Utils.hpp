#pragma once

/********************************/
/*		Libraries				*/
/********************************/
#include <iostream>
#include <string>
#include "Client.hpp"
#include <ctime>
#include <fstream>
#include "Messages.hpp"
#include <cstdlib>

/********************************/
/*		Utility functions		*/
/********************************/

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
 * @return true if the message was added
 * @return false if the message wasn't added
 */
bool addFileLog(std::string message);