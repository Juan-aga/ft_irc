#include "Server.hpp"

class Client
{
private:
    bool _authpass;
    std::string _recvBuff;
public:

    Client(void);
    ~Client(void);

    void setRecvBuff(std::string &buffer);
    std::string &getRecvBuff(void);
};