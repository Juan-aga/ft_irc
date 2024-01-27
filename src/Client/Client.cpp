#include "Client.hpp"

Client::Client(void): _recvBuff(""), fd(0), nick(""), user(""), realName(""), host(""), status(UNKNOWN)
{
	std::cout << "Client constructor called.\n";
}

Client::Client( Client const & cli )
{
	if (this != &cli)
	{
		this->fd = cli.fd;
		this->nick = cli.nick;
	}
	// return *this;
}

Client::~Client(void) 
{
	std::cout << "Client destructor called.\n";
}

void Client::setRecvBuff(std::string &buffer)
{
    this->_recvBuff = buffer;
}
std::string &Client::getRecvBuff(void)
{
    return (this->_recvBuff);
}
