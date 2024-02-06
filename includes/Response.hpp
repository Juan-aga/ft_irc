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
	RPL_NOTOPIC = 331,
	RPL_TOPIC = 332,
	RPL_INVITING = 341,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	ERR_HOST = 396,
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHCHANNEL = 403,
	ERR_CANNOTSENDTOCHAN = 404,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_USERNOTINCHANNEL = 441,
	ERR_NOTONCHANNEL = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464,
	ERR_CHANOPRIVSNEEDED = 482,
	ERR_USERSDONTMATCH = 502
};

// the trailer is the post colon (:) message
// the struct of the REPLY is :<FROM> <CODE> <TO> <COMMAND> <PARAMETERS> :<TRAILER>
// the struct of the MESSAGE is :<FROM> <COMMAND> <PARAMETERS> :<TRAILER>

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
		void Broadcast(std::vector<Client *> clients, bool self);
		//bradcast to all the channels were the cient is on. First check that channels size is greather than 0;
		void Broadcast(std::map< Channel *, std::string >	channels, bool self);
};
