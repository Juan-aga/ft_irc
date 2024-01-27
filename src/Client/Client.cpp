#include "Client.hpp"

Client::Client(void): _recvBuff(""), fd(0), nick(""), user(""), realName(""), host(""), status(UNKNOWN) {}

Client::~Client(void) {}

void Client::setRecvBuff(std::string &buffer)
{
    this->_recvBuff = buffer;
}
std::string &Client::getRecvBuff(void)
{
    return (this->_recvBuff);
}
