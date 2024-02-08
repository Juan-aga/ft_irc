#include "Client.hpp"

#include "Server.hpp"

Client::Client(void): recvBuff(""), fd(0), nick(""), user(""), realName(""), status(UNKNOWN)
{
	Server::numClients += 1;
}

Client::Client( Client const & cli )
{
	Server::numClients += 1;
	if (this != &cli)
	{
		this->fd = cli.fd;
		this->nick = cli.nick;
	}
}

Client::~Client(void)
{
	Server::numClients -= 1;
}

void Client::setRecvBuff(std::string &buffer)
{
    this->recvBuff = buffer;
}
std::string &Client::getRecvBuff(void)
{
    return (this->recvBuff);
}
