#include "Client.hpp"

#include "Server.hpp"

Client::Client(void): _recvBuff(""), fd(0), nick(""), user(""), realName(""), host(""), status(UNKNOWN)
{
	Server::numClients += 1;
	//std::cout << "Client constructor called.\n";
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
	//std::cout << "Client destructor called.\n";
}

void Client::setRecvBuff(std::string &buffer)
{
    this->_recvBuff = buffer;
}
std::string &Client::getRecvBuff(void)
{
    return (this->_recvBuff);
}
