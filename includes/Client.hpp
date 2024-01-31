#pragma once

#include "Utils.hpp"
#include "Channel.hpp"

class Server;
class Channel;

class Client
{
private:
	std::string _recvBuff;
	
public:
	int         fd;
	std::string nick;
	std::string user;
	std::string realName;
	std::string ip;
	//maybe we change the string for the enum of modes
	std::map< Channel *, std::string >	channels;
	CLIENT_STATUS   status;

	Client(void);
	Client( Client const & cli );
	~Client(void);

	void setRecvBuff(std::string &buffer);
	std::string &getRecvBuff(void);
};