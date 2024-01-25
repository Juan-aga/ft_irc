#pragma once

#include "Utils.hpp"
#include "Commands.hpp"
#include "Client.hpp"

#define MAX_CONNECTS 10
/**
 * @brief main class to manage the server
 * 
 */
class Server
{
	private:
		std::string _password;
		int 		_port;
        int         _socket_fd;
		int			_clientFd;
		Commands	_commands;
		Client		_client;

	public:
		std::string	serverName;
		std::string	serverHost;
		std::map<std::string, Client>	clients;

		Server(int port, std::string password);
		~Server();

		int 		const &getPort() const;
		std::string const &getPassword() const;

        void createSocket(void);
        void connectClient(void);
		void readMesage(int &clientFd);
		void newClient(std::vector<struct pollfd> &pollfds);
};

std::ostream &	operator<<(std::ostream & o, Server const & server);