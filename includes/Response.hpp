#pragma once

#include "Client.hpp"
#include "Utils.hpp"
//#include "Server.hpp"

class Server;

enum	Type
{
	REPLY,
	MESSAGE,
};

enum	Code
{
	NONE = 0,
	RPL_WELCOME = 1,
	RPL_NAMEREPLY = 353,
	RPL_ENDOFNAMES = 366,
};

class Response
{
	private:
		std::string from;
		std::string to;
		Type		type;
		Code		code;
		std::string content;
		int			sentfd;

		std::string generateMessage();
		std::string generateReply();
	public:
		Response();
		~Response();

		static Response createMessage();
		static Response createReply(const Code &code, std::string from);		
		Response& From(const Client &client);		
		Response& To(const Client & client);		
		Response& Content(const std::string &content);
		void Send();
};
