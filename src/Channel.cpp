#include "Channel.hpp"

#include "Server.hpp"

Channel::Channel( std::string const & name, Client * client ): name(name)
{
	clients[client->fd] = "@";
}

Channel::~Channel( void )
{
	
}

bool	Channel::addClient( Client * client )
{
	// we have to check if it's in chennel?
	// "" is the mode in the channel
	clients[client->fd] = "";
	return true;
}

bool	Channel::delClient( Client * client )
{
	clients.erase(client->fd);
	return true;
}

bool	Channel::isClient( std::string const & nick, Server & server)
{
	std::map< int, std::string >::iterator	it;
	//we have to decide how to implement clients map
	//(void)nick;
	// std::map< Client *, std::string >::iterator	it;
	
	it = this->clients.begin();
	for (; it != clients.end(); it++)
	{
		if (server.clients[it->first]->nick == nick)
			break;
	}
	if (it != clients.end())
		return true;
	else
		return false;
}

std::map< int , std::string >	Channel::getClients( void ) const
{
	return clients;
}