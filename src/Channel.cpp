#include "Channel.hpp"

#include "Server.hpp"

int Channel::totalCount = 0;

Channel::Channel( void ): name("")
{

}

Channel::Channel( std::string const & name, Client * client, Server const & server ): _numClients(1), name(name)
{
    std::string msg;

	this->topic = "";
	client->channels[this] = "@";
	clients.push_back(client);
	Server::numChannels += 1;
	totalCount += 1;
	msg = getNamereply();
	Response::createMessage().From(*client).To(*client).Command("JOIN " + name + " " + client->channels[this]).Send();
    Response::createReply(RPL_NAMREPLY).From(server).To(*client).Command("= " + name).Trailer(msg).Send();
    Response::createReply(RPL_ENDOFNAMES).From(server).To(*client).Command(name).Trailer("End of name list.").Send();
}

Channel::~Channel( void )
{
	Server::numChannels -= 1;
}

bool	Channel::addClient( Client * client, Server const & server )
{
    std::string msg;

	client->channels[this] = "+";
	clients.push_back(client);
	_numClients += 1;
	msg = getNamereply();
	Response::createMessage().From(*client).Command("JOIN " + name + " " + client->channels[this]).Broadcast(clients, true);
	Response::createReply(RPL_TOPIC).From(server).To(*client).Command(name).Trailer(this->topic).Send();
    Response::createReply(RPL_NAMREPLY).From(server).To(*client).Command("= " + name).Trailer(msg).Send();
    Response::createReply(RPL_ENDOFNAMES).From(server).To(*client).Command(name).Trailer("End of name list.").Send();
	return true;
}

bool	Channel::delClient( Client * client, Server const & server )
{
	(void)server;

	client->channels.erase(this);
	for (std::vector< Client *>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (*it == client)
		{
			clients.erase(it);
			break;
		}
	}
	if (--_numClients <= 0)
	{}//we need to delete this channel.. maybe create a function on the server?
	// server need to broadcast that the client was quit.
	return true;
}

bool	Channel::isClient( std::string const & nick)
{
	std::map< Client*, std::string >::iterator	it;

	for (std::vector< Client *>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if ((*it)->nick == nick)
			return true;
	}
	return false;
}

std::string							Channel::getNamereply( void )
{
	std:: string	msg = "";

	// we have to show the clients depending on this modes. We need to apply fiters.
	for (std::vector< Client *>::iterator gclients = clients.begin(); gclients != clients.end(); gclients++)
		msg += (*gclients)->channels[this] + (*gclients)->nick + " ";
	return msg;
}