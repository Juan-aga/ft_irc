#include "includes/server.hpp"

Server::Server(int port, std::string password): _port(port), _password(password)
{
    //debug can eliminate this
    std::cout << "Server constructor called" << std::endl;

}

Server::~Server()
{
    //debug can eliminate this
    std::cout << "Server destructor called" << std::endl;
}

//PORT
int Server::getPort(void)
{
    return (this->_port);
}

void Server::setPort(int port)
{
    this->_port = port;
}

//PASSWORD
std::string Server::getPassword(void)
{
    return (this->_password);
}

void Server::setPassword(std::string password)
{
    this->_password = password;
}

void Server::createSocket()
{
    int fdSocket;
    sockaddr_in socketAddress;

    //CREACION DEL SOCKET
    fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSocket == -1)
    {
        std::cerr << "Error creating the socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = INADDR_ANY; //establece como ip por defecto 0.0.0.0
    socketAddress.sin_port = htons(this->_port);

    std::cout << "hola; " << fdSocket << std::endl;
    if (bind(fdSocket, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
    {
        perror("binding");
        close(fdSocket);
        exit(EXIT_FAILURE);
    }
    std::cout << "listening" << std::endl;
    if (listen(fdSocket, MAX_CONNECTS) == -1)
    {
        perror("listening");
        close(fdSocket);
        exit(EXIT_FAILURE);
    }

    this->_socket_fd = fdSocket;
}