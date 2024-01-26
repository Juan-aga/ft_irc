#pragma once

#include "Client.hpp"
#include "Utils.hpp"
#include "Response.hpp"

class Server;

class Response;

class Commands
{
	private:
		enum    _CMD
		{
			CAP,
			PASS,
			NICK,
			USER,
			JOIN,
			MAX_CMD
		};
		typedef bool (*cmdFunction)(const std::string &, Client &, Server &);
		typedef struct s_commands
		{
			cmdFunction exec;
		} t_commands;
		t_commands  commands[MAX_CMD];

		std::map<std::string, _CMD> commandMap;

		Commands::_CMD  strToCmd( const std::string & cmd );

		bool        checkLogin( Client & client );

		static bool execCap( const std::string & argument, Client & client, Server & server );
		static bool execPass( const std::string & argument, Client & client, Server & server );
		static bool execNick( const std::string & argument, Client & client, Server & server );
		static bool execUser( const std::string & argument, Client & client, Server & server );
		static bool execJoin( const std::string & argument, Client & client, Server & server );

	public:
		Commands( void );
		~Commands( void );

		bool    processInput( const std::string & input, Client & client, Server & server );
		bool    execCmd( const std::string & command, const std::string & argument, Client & client, Server & server );

};