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
		std::string	topic;
		std::vector< Client *>	clients;
		//std::map< Client * , std::string >	clients;
		//the chanel must have more options...

		//we need to add topic and RPL_TOPIC (332) when add user.
		bool isFull;
		bool inviteOnly;
		bool opTopic;

		std::vector< Client *>	inviteList;
		Channel( void );
		Channel( std::string const & name, Client * client, Server const & server );
		~Channel( void );
		
		static bool	validName( std::string const & name );
		bool	addClient( Client * client, Server const & server );
		bool	delClient( Client * client, Server & server );
		bool	isClient( std::string const & nick );
		bool	isInvite( Client * client );

		std::string							getNamereply( void );
};