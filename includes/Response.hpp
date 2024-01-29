#pragma once

#include "Client.hpp"
#include "Utils.hpp"

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
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	ERR_HOST = 396,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432, 
	ERR_NICKNAMEINUSE = 433,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464
};

// the trailer is the post colon (:) message
// the struct of the REPLY is :<FROM> <CODE> <TO> <COMMAND> <PARAMETERS> :<TRAILER>
// the struct of the MESSAGE is :<FROM> <COMMAND> <PARAMETERS> :<TRAILER>
// we need a broadcast for channel (maybe for server too?, we can handle with overload) and a flag (bool self?) to send messages to all channel and/not to the sender.
// we add in command command and parameters if we needed.

// in a client to server model (not server to server comunication) we can use only the nick (not <NICK>!<HOST>)

class Response
{
	private:
		std::string from;
		std::string to;
		Type		type;
		Code		code;
		std::string	command;
		std::string trailer;
		int			sentfd;

		std::string generateMessage();
		std::string generateReply();
	public:
		Response();
		~Response();

		static Response createMessage();
		static Response createReply(const Code &code);		
		Response& From(const Client &client);
		Response& From(const Server & server);	
		Response& To(const Client & client);
		Response &	Command( const std::string & command );
		Response& Trailer(const std::string &trailer);
		void Send();
		void Broadcast(std::map<Client *, std::string> clients, bool self);

		//we need to implement a broadcast to al clients in a channel, when channel class has been implemented.
		//it could be a To channel.
};
