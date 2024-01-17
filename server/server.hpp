#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_CONNECTS 10

class server
{
private:
    //external data
    int _port; //listening port
    int _socket_fd;
public:
    server(int port, std::string password);
    ~server();

    /*getters and setters*/
    //port
    int getPort(void);
    void setPort(int port);
    //password
    std::string getPassword(void);
    void setPassword(std::string password);

    //other methods
    int initServer(void); //init the server socket and configure it to start listening
    int createSocket(void); //create the socket and return th file descriptor
};


