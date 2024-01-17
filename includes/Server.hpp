#pragma once

#include "Utils.hpp"

/**
 * @brief main class to manage the server
 * 
 */
class Server
{
	private:
		std::string password;
		int 		port;
	public:
		Server(std::string password, int port);
		~Server();

		int 		const &getPort() const;
		std::string const &getPassword() const;
};