#include "../includes/Server.hpp"

int main(int argc, char **argv)
{
	checkArgs(argc, argv);
	Server server(atoi(argv[1]), argv[2]);
	
	std::cout << "Server password: " << server.getPassword() << std::endl;
	std::cout << "Server port: " << server.getPort() << std::endl;
	server.createSocket();
	while(1)
		server.connectClient();
	return (0);
}