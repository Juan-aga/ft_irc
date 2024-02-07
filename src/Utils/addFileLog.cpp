#include "Utils.hpp"
#include <stdlib.h>

bool addFileLog(std::string message, std::string color) {
	(void)color;
	std::ofstream file("ircserv.log", std::ios::app);
	std::string time = getTime();
	if (!file.is_open()) {
		std::cerr << time << RED_CMD << "[-]Error opening file ircserv.log" << std::endl;
		return false;
	}
	file << time << " " << message << std::endl;
	file.close();
	//std::cout << getTime() << color << message << NOCOLOR_CMD << std::endl;
	return true;
}
