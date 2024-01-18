#pragma once

/********************************/
/*		Libraries				*/
/********************************/
#include <iostream>
#include <string>
#include "Client.hpp"
#include "Messages.hpp"

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