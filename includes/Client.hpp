#pragma once

#include "Utils.hpp"
#include "Channel.hpp"

class Server;
class Channel;

class Client
{
	private:

	public:
		std::string recvBuff;
		int         fd;
		std::string nick;
		std::string user;
		std::string realName;
		std::string ip;
		std::map< Channel *, std::string >	channels;
		CLIENT_STATUS   status;

		Client(void);
		Client( Client const & cli );
		~Client(void);

		void setRecvBuff(std::string &buffer);
		std::string &getRecvBuff(void);
};
