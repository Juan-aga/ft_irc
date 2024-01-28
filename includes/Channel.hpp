#pragma once

#include "Utils.hpp"
// //#inclide "Utils.hpp"
// #inclide "Clients.hpp"
#include "Client.hpp"

//#include <map>

class Server;

class Channel
{
	private:
		// const Server &	server;
		
	public:
		static int	totalCount;

		std::string	name;
		//this is file descriptor of clients and mode in the channel
		//std::map< int, std::string >	clients;
		std::map< Client * , std::string >	clients;
		//the chanel must have more options...
		
		Channel( void );
		Channel( std::string const & name, Client * client );
		~Channel( void );
		
		//maybe pass the mode for this channel, or assign here???
		bool	addClient( Client * client );
		bool	delClient( Client * client );
		bool	isClient( std::string const & nick );
		
		std::map< Client *, std::string >	getClients( void ) const;
		std::string							getNamereply( void );
		
};