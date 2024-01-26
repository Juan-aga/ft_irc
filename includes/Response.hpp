#pragma once

#include "Client.hpp"
#include "Utils.hpp"
#include "Server.hpp"

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

		string generateMessage();
		string generateReply();
	public:
		Response();
		~Response();

		static Response createMessage();
		static Response createReply(const Code &code);		
		Response& from(const Client &client);		
		Response& to(const Client & client);		
		Response& content(const string &content);
		void send();
};
