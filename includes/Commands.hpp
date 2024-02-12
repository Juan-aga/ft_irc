#pragma once

#include "Client.hpp"
#include "Utils.hpp"
#include "Response.hpp"

class Server;

class Commands
{
	private:
		enum    _CMD
		{
			//only UNKNOWN
			PASS,
			//only AUTH
			CAP,
			WHO,
			NICK,
			USER,
			WHOIS,
			QUIT,
			//only CONNECTED
			JOIN,
			PRIVMSG,
			TOPIC,
			PART,
			KICK,
			INVITE,
			//to close the erver.
			KILLSERVER,
			//mode
			MODE,
			MAX_CMD
		};
		typedef void (*cmdFunction)(const std::string &, Client *, Server &);
		typedef struct s_commands
		{
			cmdFunction exec;
		} t_commands;
		t_commands  commands[MAX_CMD];

		std::map<std::string, _CMD> commandMap;

		Commands::_CMD  strToCmd( const std::string & cmd );

		bool        checkLogin( Client * client, Server const & server );
		static bool	parseNick( const std::string & parameter);

		//connection
		static void execCap( const std::string & parameter, Client * client, Server & server );
		static void execWho( const std::string & parameter, Client * client, Server & server );
		static void execPass( const std::string & parameter, Client * client, Server & server );
		static void execNick( const std::string & parameter, Client * client, Server & server );
		static void execUser( const std::string & parameter, Client * client, Server & server );
		static void execWhois( const std::string & parameter, Client * client, Server & server );
		static void execQuit( const std::string & parameter, Client * client, Server & server );
		//channels
		static void execJoin( const std::string & parameter, Client * client, Server & server );
		static void execPrivmsg( const std::string & parameter, Client * client, Server & server );
		static void execTopic( const std::string & parameter, Client * client, Server & server );
		static void execPart( const std::string & parameter, Client * client, Server & server );
		static void execKick( const std::string & parameter, Client * client, Server & server );
		static void execInvite( const std::string & parameter, Client * client, Server & server );

		static void execKill( const std::string & parameter, Client * client, Server & server );
		static void	execMode(const std::string & parameter, Client * client, Server & server);

	public:
		Commands( void );
		~Commands( void );

		void    processInput( const std::string & input, Client * client, Server & server );
		void    execCmd( const std::string & command, const std::string & parameter, Client * client, Server & server );

};
