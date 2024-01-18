#include "Utils.hpp"

bool addFileLog(std::string message) {
	std::ofstream file("ircserv.log", std::ios::app);
	if (!file.is_open()) {
		std::cerr << "Error opening file ircserv.log" << std::endl;
		return false;
	}
	file << getTime() << " " << message << std::endl;
	file.close();
	return true;
}