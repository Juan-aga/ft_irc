#pragma once

#include "Utils.hpp"
// //#inclide "Utils.hpp"
// #inclide "Clients.hpp"
#include "Client.hpp"

//#include <map>

class Server;
class Client;

class Channel
{
	private:
		int			_numClients;

	public:
		static int	totalCount;

		std::string	name;
		std::vector< Client *>	clients;
		//std::map< Client * , std::string >	clients;
		//the chanel must have more options...
		
		Channel( void );
		Channel( std::string const & name, Client * client, Server const & server );
		~Channel( void );
		
		bool	addClient( Client * client, Server const & server );
		bool	delClient( Client * client, Server const & server );
		bool	isClient( std::string const & nick );
		
		std::string							getNamereply( void );
};