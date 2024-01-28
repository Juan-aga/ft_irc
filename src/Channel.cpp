#include "Channel.hpp"

#include "Server.hpp"

int Channel::totalCount = 0;

Channel::Channel( void ): name("")
{

}

Channel::Channel( std::string const & name, Client * client ): name(name)
{
	clients[client] = "@";
	Server::numChannels += 1;
	totalCount += 1;
}

Channel::~Channel( void )
{
	Server::numChannels -= 1;
}

bool	Channel::addClient( Client * client )
{
	// we have to check if it's in chennel?
	// "@" is the mode in the channel all operator to test.

	clients[client] = "+";
	return true;
}

bool	Channel::delClient( Client * client )
{
	clients.erase(client);
	return true;
}

bool	Channel::isClient( std::string const & nick)
{
	std::map< Client*, std::string >::iterator	it;

	it = this->clients.begin();
	for (; it != this->clients.end(); it++)
	{
		if (it->first->nick == nick)
			break;
	}
	if (it != this->clients.end())
		return true;
	else
		return false;
}

std::map< Client *, std::string >	Channel::getClients( void ) const
{
	return clients;
}

std::string							Channel::getNamereply( void )
{
	std:: string	msg = "";

	for (std::map<Client *, std::string>::iterator gclients = clients.begin(); gclients != clients.end(); gclients++)
		msg += gclients->second + gclients->first->nick + " ";
	return msg;
}