#include "Channel.hpp"

#include "Server.hpp"

int Channel::totalCount = 0;

Channel::Channel( void ): name("")
{

}

Channel::Channel( std::string const & name, Client * client, Server const & server ): name(name)
{
    std::string msg;

	clients[client] = "@";
	Server::numChannels += 1;
	totalCount += 1;
	msg = getNamereply();
	Response::createMessage().From(*client).To(*client).Command("JOIN " + name + " " + clients[client]).Send();
    Response::createReply(RPL_NAMREPLY).From(server).To(*client).Command("= " + name).Trailer(msg).Send();
    Response::createReply(RPL_ENDOFNAMES).From(server).To(*client).Command(name).Trailer("End of name list.").Send();

}

Channel::~Channel( void )
{
	Server::numChannels -= 1;
}

bool	Channel::addClient( Client * client, Server const & server )
{
	// we have to check if it's in chennel?
	// "@" is the mode in the channel all operator to test.
    std::string msg;

	clients[client] = "+";
	msg = getNamereply();
	for (std::map<Client *, std::string>::iterator gclients = clients.begin(); gclients != clients.end(); gclients++)
		Response::createMessage().From(*client).To(*gclients->first).Command("JOIN " + name + " " + clients[client]).Send();
    Response::createReply(RPL_NAMREPLY).From(server).To(*client).Command("= " + name).Trailer(msg).Send();
    Response::createReply(RPL_ENDOFNAMES).From(server).To(*client).Command(name).Trailer("End of name list.").Send();
	return true;
}

bool	Channel::delClient( Client * client, Server const & server )
{
	(void)server;
	clients.erase(client);
	// server need to broadcast that the client was quit.
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