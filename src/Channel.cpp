#include "Channel.hpp"

#include "Server.hpp"

int Channel::totalCount = 0;

Channel::Channel( void ): _numClients(1), name("#general"), topic("This is the general channel.")
{
	Server::numChannels += 1;
	this->password = "";
	this->clientLimit = 0;
	this->isFull = false;
	this->inviteOnly = false;
	this->opTopic = true;
	this->needPass = false;
	this->isLimited = false;
}

Channel::Channel( std::string const & name, Client * client, Server const & server ): _numClients(1), name(name)
{
    std::string msg;

	if (name == "#General")
		this->topic = "This is the general channel.";
	else
		this->topic = "";
	this->password = "";
	this->clientLimit = 0;
	this->isFull = false;
	this->inviteOnly = false;
	this->opTopic = true;
	this->needPass = false;
	this->isLimited = false;
	client->channels[this] = "@";
	clients.push_back(client);
	Server::numChannels += 1;
	totalCount += 1;
	msg = getNamereply();
	Response::createMessage().From(*client).To(*client).Command("JOIN " + name).Send("[+]Client: " + client->nick + " created channel: " + this->name, GREEN_CMD);
    Response::createReply(RPL_NAMREPLY).From(server).To(*client).Command("= " + name).Trailer(msg).Send();
    Response::createReply(RPL_ENDOFNAMES).From(server).To(*client).Command(name).Trailer("End of name list.").Send();
}

Channel::~Channel( void )
{
	Server::numChannels -= 1;
}

bool	Channel::validName( std::string const & name )
{
	if (name[0] != '#' || !name[1])
		return false;
	for (size_t i = 1; i < name.size(); i++)
	{
		if (name[i] == ' ' || name[i] == 7 || name[i] == ',')
			return false;
	}
	return true;
}

bool	Channel::addClient( Client * client, Server const & server, std::string const & password )
{
    std::string msg;

    if ((needPass && this->password == password) || !needPass)
    {
		client->channels[this] = "+";
		clients.push_back(client);
		_numClients += 1;
		msg = getNamereply();
		Response::createMessage().From(*client).Command("JOIN " + name).Broadcast(clients, true);
		if (this->topic != "")
			Response::createReply(RPL_TOPIC).From(server).To(*client).Command(name).Trailer(this->topic).Send();
		else
			Response::createReply(RPL_NOTOPIC).From(server).To(*client).Command(name).Trailer("No topic is set").Send();
	    Response::createReply(RPL_NAMREPLY).From(server).To(*client).Command("= " + name).Trailer(msg).Send();
	    Response::createReply(RPL_ENDOFNAMES).From(server).To(*client).Command(name).Trailer("End of name list.").Send("[+]Client: " + client->nick + " joined channel: " + this->name, GREEN_CMD);
		return true;
    }
    Response::createReply(ERR_BADCHANNELKEY).From(server).To(*client).Command(name).Trailer("Cannot join channel (+k)").Send("[-]Client: " + client->nick + "  tried to join channel: " + name + " but wrong or missing keyword", RED_CMD);
    return false;
}

bool	Channel::delClient( Client * client, Server & server )
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i] == client)
		{
			clients.erase(clients.begin() + i);
			break;
		}
	}
	if (--_numClients <= 0)
		server.closeChannel(this);
	return true;
}

bool	Channel::isClient( std::string const & nick )
{
	std::map< Client*, std::string >::iterator	it;

	for (std::vector< Client *>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if ((*it)->nick == nick)
			return true;
	}
	return false;
}

bool	Channel::isInvite( Client * client )
{
	for (std::vector< Client *>::iterator it = inviteList.begin(); it != inviteList.end(); it++)
	{
		if (*it == client)
			return true;
	}
	return false;
}

std::string							Channel::getNamereply( void )
{
	std:: string	msg = "";

	for (std::vector< Client *>::iterator gclients = clients.begin(); gclients != clients.end(); gclients++)
		msg += (*gclients)->channels[this] + (*gclients)->nick + " ";
	return msg;
}

std::string							Channel::getMode( void )
{
	std::string	mode = "";

	if (this->isLimited)
		mode += "l";
	if (this->inviteOnly)
		mode += "i";
	if (this->opTopic)
		mode += "t";
	if (this->needPass)
		mode += "k";
	return mode;
}
