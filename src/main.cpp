#include "Server.hpp"

int main(int argc, char **argv)
{
	checkArgs(argc, argv);
	Server server(argv[2], atoi(argv[1]));
	
	std::cout << "Server password: " << server.getPassword() << std::endl;
	std::cout << "Server port: " << server.getPort() << std::endl;
	return (0);
}