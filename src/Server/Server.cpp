#include "Server.hpp"

int Server::numClients = 0;
int Server::numChannels = 0;
bool Server::_running = true;

Server::Server(int port, std::string password): _password(password), _port(port),serverName("server"), serverHost("test.irc")
{
	channels.push_back(new Channel());
}

Server::~Server()
{
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		close(it->second->fd);
		delete it->second;
	}
	for (std::vector< Channel * >::iterator it = channels.begin(); it != channels.end(); it++)
		delete *it;
	close(_socket_fd);
	addFileLog("[+]Server closed.", GREEN_CMD);
}

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

void Server::readMesage(Client * client)
{
	int readed = 0;
	char buffer[1024];

	memset(buffer, 0, 1024);
	while (1)
	{
		readed = recv(client->fd, buffer, 1024, 0);
		if (!readed || client->status == DISCONECT)
		{
			// maybe make a function for this?
			if (DEBUG && !readed)
				std::cout << "Connection closed by client " << client->fd << std::endl;
			close(client->fd);
			client->status = DISCONECT;
			clients.erase(client->fd);
			delete client;
			return;
		}
		if (readed == -1)
		{
			if (DEBUG)
				std::cout << "Failed to read from " << client->nick << " in fd: " << client->fd << std::endl;
			break;
		}
		client->recvBuff.append(buffer);
		if (client->recvBuff.find("\n") != std::string::npos)
		{
			if (DEBUG)
				std::cout << "Readed: " << client->recvBuff << std::endl;
			_commands.processInput(client->recvBuff, client, *this);
			client->recvBuff.clear();
			break;
		}
	}
}

void Server::newClient(std::vector<struct pollfd>& pollfds)
{
	sockaddr_in clientAddress;
	socklen_t addrlen = sizeof(clientAddress);
	int					clientFd;

	clientFd = accept(this->_socket_fd, (sockaddr *)&clientAddress, &addrlen);

	if (clientFd != -1)
	{
		fcntl(clientFd, F_SETFL, O_NONBLOCK);
		pollfds.push_back((struct pollfd){clientFd, POLLIN | POLLOUT, 0});
		Client		*client = new Client;
		// check this in case of segfault
		struct in_addr inetAddress;
		inetAddress.s_addr = clientAddress.sin_addr.s_addr;
		client->ip = inet_ntoa(inetAddress);
		client->fd = clientFd;
		clients[client->fd] = client;
	}
}

void Server::connectClient(void)
{
	std::vector<struct pollfd> pollfds;

	pollfds.push_back((struct pollfd){this->_socket_fd, POLLIN, 0});
	while (poll(&pollfds[0], pollfds.size(), -1) && _running)
	{
		for (int i = 0; i < int(pollfds.size()); i++)
		{
			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == this->_socket_fd)
					newClient(pollfds);
				else
					readMesage(clients[pollfds[i].fd]);
			}
		}
	}

}

Channel *	Server::getChannelByName( std::string const & name )
{
	for (std::vector< Channel * >::iterator it = channels.begin(); it != channels.end(); it++)
	{
		if ((*it)->name == name)
			return *it;
	}
	return NULL;
}

Client *	Server::getClientByNick( std::string const & nick )
{
	for (std::map< int, Client * >::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it->second->nick == nick)
			return it->second;
	}
	return NULL;
}

void		Server::closeChannel( Channel * channel )
{
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i] == channel)
		{
			addFileLog("[!]Channel: " + channel->name + " has been closed.",  YELLOW_CMD);
			delete channel;
			channels.erase(channels.begin() + i);
			break;
		}
	}
}

void		Server::stopServer( void )
{
	_running = false;
}

std::ostream &	operator<<(std::ostream & o, Server const & server)
{
	o << "Server Running on -> " << HOST << ":"<< server.getPort() << " with password -> " << server.getPassword() << ".";
	return (o);
}
