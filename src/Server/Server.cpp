#include "Server.hpp"

#include "../includes/Server.hpp"

Server::Server(int port, std::string password): _password(password), _port(port), _clientFd(0)
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
int const &Server::getPort(void) const
{
    return (this->_port);
}

//PASSWORD
std::string const &Server::getPassword(void) const
{
    return (this->_password);
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
    if (bind(fdSocket, (sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
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

void Server::connectClient(void)
{
    //int _clientFd;
    sockaddr_in clientAddress;
    socklen_t addrlen = sizeof(clientAddress);
    char buffer[1024]; //save the message

    if (!_clientFd)
        _clientFd = accept(this->_socket_fd, (sockaddr *)&clientAddress, &addrlen);

    if (_clientFd == -1)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    //read mesages
    int readed = 0;
    std::string readBuffer = "";
    std::string tmpBuffer = "";

    memset(buffer, 0, 1024);
    while (1)
    {
        readed = recv(_clientFd, buffer, 1024, 0);
        if (!readed)
        {
            //connection close, delete client.
            return;
        }
        if (readed == -1)
        {
            //failed to read from client.
            return;
        }
        tmpBuffer = buffer;
        readBuffer.append(tmpBuffer);
        if (readBuffer.find("\r\n"))
        {
            std::cout << "Readed: " << tmpBuffer << std::endl;
            break;
        }
    }
    std::cerr << "Readed complete.\n";
}