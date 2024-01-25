#include "Client.hpp"

Client::Client(void): _recvBuff(""), authpass(false), fd(0), nick(""), user(""), realName(""), host(""), status(UNKNOWN)
{
    std::cout << "client called" << std::endl;
}

Client::~Client(void)
{
    std::cout << "client destructor called" << std::endl;
}

void Client::setRecvBuff(std::string &buffer)
{
    this->_recvBuff = buffer;
}
std::string &Client::getRecvBuff(void)
{
    return (this->_recvBuff);
}
