#pragma once

#include "Utils.hpp"
#include "Commands.hpp"
#include "Client.hpp"
#include "Channel.hpp"

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
		Commands	_commands;
		static bool		_running;

	public:
		static int	numClients;
		static int	numChannels;

		std::string	serverName;
		std::string	serverHost;
		std::map<int, Client * >	clients;
		std::vector<Channel *>	channels;

		Server(int port, std::string password);
		~Server();

		int 		const &getPort() const;
		std::string const &getPassword() const;

        void createSocket(void);
        void connectClient(void);
        void delClient( Client * client ,std::vector<struct pollfd>& pollfds );
		void readMesage(Client * client);
		void newClient(std::vector<struct pollfd> &pollfds);
		Channel *	getChannelByName( std::string const & name );
		Client *	getClientByNick( std::string const & nick );
		void		closeChannel( Channel * channel );
		static void	stopServer( void );
		std::string bot(std::string bot_cmd);
};

std::ostream &	operator<<(std::ostream & o, Server const & server);
