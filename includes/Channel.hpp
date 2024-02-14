#pragma once

#include "Utils.hpp"
#include "Client.hpp"

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
		std::vector< Client *>	inviteList;
		int	clientLimit;
		bool isFull;
		bool inviteOnly;
		bool opTopic;
		bool needPass;
		bool isLimited;

		Channel( void );
		Channel( std::string const & name, Client * client, Server const & server );
		~Channel( void );

		static bool	validName( std::string const & name );
		bool	addClient( Client * client, Server const & server, std::string const & password );
		bool	delClient( Client * client, Server & server );
		bool	isClient( std::string const & nick );
		bool	isInvite( Client * client );

		std::string							getNamereply( void );
		std::string							getMode( void );
};
