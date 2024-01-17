#include "Utils.hpp"

static bool isNumber(std::string str) {
	if (str.empty())
		return false;
	for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
	{
		if (!std::isdigit(*it))
			return false;
	}
	return true;
}

void checkArgs(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << ERROR_ARGV_1;
		exit(1);
	}	
	if (!isNumber(argv[1]))
	{
		std::cerr << ERROR_ARGV_2;
		exit(1);
	}
	if (atoi(argv[1]) < 1024 || atoi(argv[1]) > 65535)
	{
		std::cerr << ERROR_ARGV_3;
		exit(1);
	}
}