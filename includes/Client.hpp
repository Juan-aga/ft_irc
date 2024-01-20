#pragma once

#include <iostream>
class Client
{
private:
    std::string _recvBuff;
public:
    bool _authpass;

    Client(void);
    ~Client(void);

    void setRecvBuff(std::string &buffer);
    std::string &getRecvBuff(void);
};