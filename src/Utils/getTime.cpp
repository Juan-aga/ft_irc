#include "Utils.hpp"

std::string getTime() {
	time_t now = time(nullptr);
	struct tm* tm_now = localtime(&now);
	char buffer[80];
	strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", tm_now);
	return std::string(buffer);
}
