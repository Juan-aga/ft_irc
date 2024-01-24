#pragma once

#include "Utils.hpp"
class Client
{
private:
    std::string _recvBuff;
    
public:
    bool        authpass;
    int         fd;
    std::string nick;
    std::string user;
    std::string realName;

    Client(void);
    ~Client(void);

    void setRecvBuff(std::string &buffer);
    std::string &getRecvBuff(void);
};