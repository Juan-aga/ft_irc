#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_CONNECTS 10

class Server
{
private:
    //external data
    int _port; //listening port
    std::string _password; //connection password
    int _socket_fd;
public:
    Server(int port, std::string password);
    ~Server();

    /*getters and setters*/
    //port
    int getPort(void);
    void setPort(int port);
    //password
    std::string getPassword(void);
    void setPassword(std::string password);

    //other methods
    void createSocket(void); //create the socket and return th file descriptor
};


