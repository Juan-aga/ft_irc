#pragma once

#include "Client.hpp"
#include "Utils.hpp"

// for check fstat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
	RPL_WHOISUSER = 311,
	RPL_ENDOFWHOIS = 318,
	RPL_CHANNELMODEIS = 324,
	RPL_NOTOPIC = 331,
	RPL_TOPIC = 332,
	RPL_INVITING = 341,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	ERR_HOST = 396,
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHCHANNEL = 403,
	ERR_CANNOTSENDTOCHAN = 404,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_USERNOTINCHANNEL = 441,
	ERR_NOTONCHANNEL = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464,
	ERR_KEYSET = 467,
	ERR_CHANNELISFULL = 471,
	ERR_INVITEONLYCHAN = 473,
	ERR_BADCHANNELKEY = 475,
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
		CLIENT_STATUS	toStatus;
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
		void Send(std::string message, std::string color);
		void Broadcast(std::vector<Client *> clients, bool self);
		//bradcast to all the channels were the cient is on. First check that channels size is greather than 0;
		void Broadcast(std::map< Channel *, std::string >	channels, bool self);
};
