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
		std::string password;
		std::vector< Client *>	clients;
		int	clientLimit;
		//std::map< Client * , std::string >	clients;
		//the chanel must have more options...

		bool isFull;
		bool inviteOnly;
		bool opTopic;
		bool needPass;
		bool isLimited;

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
		std::string							getMode( void );
};