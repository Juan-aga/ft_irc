#include "Server.hpp"

Server::Server(int port, std::string password): _password(password), _port(port), _clientFd(0), serverName("server"), serverHost("test.irc") {}

Server::~Server() {}

int const &Server::getPort(void) const
{
	return (this->_port);
}

std::string const &Server::getPassword(void) const
{
	return (this->_password);
}

void Server::createSocket()
{
	int fdSocket;
	int opt = 1;
	sockaddr_in socketAddress;

	//create socket
	fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (fdSocket == -1)
	{
		addFileLog(" [-]Error creating the socket", RED_CMD);
		exit(EXIT_FAILURE);
	}

	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY;
	socketAddress.sin_port = htons(this->_port);

	//set socket options
	if (setsockopt(fdSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		addFileLog(" [-]Error setting socket options", RED_CMD);
		close(fdSocket);
		exit(EXIT_FAILURE);
	}
	//bind ip and port to socket
	if (bind(fdSocket, (sockaddr *)&socketAddress, sizeof(socketAddress)) == -1)
	{
		addFileLog(" [-]Error binding the socket", RED_CMD);
		close(fdSocket);
		exit(EXIT_FAILURE);
	}
	//listen for connections
	if (listen(fdSocket, MAX_CONNECTS) == -1)
	{
		addFileLog(" [-]Error listening the socket", RED_CMD);
		close(fdSocket);
		exit(EXIT_FAILURE);
	}
	this->_socket_fd = fdSocket;
}

void Server::readMesage(int &clientFd)
{
	int readed = 0;
	std::string readBuffer = "";
	std::string tmpBuffer = "";
	char buffer[1024]; //save the message

	memset(buffer, 0, 1024);
	while (1)
	{
		readed = recv(clientFd, buffer, 1024, 0);
		if (!readed)
		{
			//connection close, delete client.
			std::cout << "Connection closed by client " << clientFd << std::endl;
			close(clientFd);
			break;
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
	_client.fd = clientFd;
	// process input handle in other function
	// this may change with poll
	_commands.processInput(readBuffer, _client, *this);
}

void Server::newClient(std::vector<struct pollfd>& pollfds)
{
    sockaddr_in clientAddress;
    socklen_t addrlen = sizeof(clientAddress);

    this->_clientFd = accept(this->_socket_fd, (sockaddr *)&clientAddress, &addrlen);

    if (this->_clientFd != -1)
    {
        fcntl(this->_clientFd, F_SETFL, O_NONBLOCK);
        pollfds.push_back((struct pollfd){this->_clientFd, POLLIN | POLLOUT, 0});
    }
}

void Server::connectClient(void)
{
	std::vector<struct pollfd> pollfds;
	pollfds.push_back((struct pollfd){this->_socket_fd, POLLIN, 0});

	while (poll(&pollfds[0], pollfds.size(), -1))
	{
		for (int i = 0; i < int(pollfds.size()); i++)
		{
			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == this->_socket_fd)
					newClient(pollfds);
				else
					readMesage(pollfds[i].fd);
			}
		}
	}
	
}            

std::ostream &	operator<<(std::ostream & o, Server const & server)
{
	o << "Server Running on -> " << HOST << ":"<< server.getPort() << " with password -> " << server.getPassword() << ".";
	return (o);
}