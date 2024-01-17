#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "Utils.hpp"

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
	public:
		Server(int port, std::string password);
		~Server();

		int 		const &getPort() const;
		std::string const &getPassword() const;

        void createSocket(void);
        void connectClient(void);
};