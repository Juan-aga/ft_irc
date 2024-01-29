#include "Server.hpp"

int main(int argc, char **argv)
{
	checkArgs(argc, argv);
	Server server(atoi(argv[1]), argv[2]);

	ft_init_signals();
	server.createSocket();
	std::stringstream ss;
	ss << "[+]" << server;
	addFileLog(ss.str(), GREEN_CMD);
	server.connectClient();
	return (0);
}