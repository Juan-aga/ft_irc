#include "server.hpp"

server::server(int port, std::string password): _port(port), _password(password)
{
    //debug can eliminate this
    std::cout << "Server constructor called" << std::endl;
    std::cout << "Server constructor called" << std::endl;

    this->_socket_fd = createSocket();
}

server::~server()
{
    //debug can eliminate this
    std::cout << "Server destructor called" << std::endl;
}

//PORT
int server::getPort(void)
{
    return (this->_port);
}

void server::setPort(int port);
{
    this->_port = port;
}

//PASSWORD
std::string server::getPassword(void)
{
    return (this->_password);
}

void server::setPassword(std::string password)
{
    this->_password = password;
}

int server::initServer(void)
{

}

int server::createSocket()
{
    int fdSocket;
    sockaddr_in socketAddress;

    //CREACION DEL SOCKET
    fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSocket == -1)
    {
        std::cerr << "Error creating the socket" << std::endl;
        return (-1);
    }

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = INADDR_ANY; //establece como ip por defecto 0.0.0.0
    socketAddress.sin_port = htons(this->_port);

    if (bind(fdSocket, (sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
    {
        std::cerr << "Error binding the socket" << std::endl;
        return (-1);
    }

    if (listen(fdSocket, MAX_CONNECTS) == -1)
    {
        std::cerr << "Error binding the socket" << std::endl;
        return (-1);
    }

    return (fdSocket);
}