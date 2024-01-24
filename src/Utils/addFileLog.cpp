#include "Utils.hpp"

bool addFileLog(std::string message, std::string color) {
	std::ofstream file("ircserv.log", std::ios::app);
	if (!file.is_open()) {
		std::cerr << getTime() << RED_CMD << "[-]Error opening file ircserv.log" << std::endl;
		return false;
	}
	file << getTime() << " " << message << std::endl;
	file.close();
	std::cout << getTime() << color << message << NOCOLOR_CMD << std::endl;
	return true;
}