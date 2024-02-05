#include "Commands.hpp"
#include "Server.hpp"

//Operators commands:
//	KICK
//	INVITE
//	TOPIC
//	MODE

//we have to add this channels mode:
//  Client Limit Channel Mode   +l
//  Invite-Only Channel Mode    +i
//  Key Channel Mode            +k
//  Protected Topic Mode        +t

//clients mode only:
//  Operator:	prefix @		+o
//  regular :	prefix +		+v

void        Commands::execJoin( const std::string & parameter, Client * client, Server & server )
{
	Channel *   channel;
	
	channel = server.getChannelByName(parameter);
	// If parameter is "0", the client leave all channels. execute PART for every channel.
	if (parameter.size() == 1 && parameter[0] == '0')
	{
		for (std::map< Channel *, std::string >::iterator chan = client->channels.begin(); client->channels.size() > 0; chan = client->channels.begin())
			execPart(chan->first->name + " " , client, server);
	}
	else if (!Channel::validName(parameter))
	{
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("JOIN").Trailer("Not enough parameters").Send();
		addFileLog("[-]Client: " + client->nick + " tried to join an invalid channel: " + parameter, RED_CMD);
	}
	else if (channel)
	{
		if (channel->isClient(client->nick))
			addFileLog("[!]Client: " + client->nick + " is already a member of channel: " + parameter, YELLOW_CMD);
		//we have to check permmisions
		// send ERR_INVITEONLYCHAN (473)
		else
		{
			addFileLog("[+]Client: " + client->nick + " joined channel: " + parameter, GREEN_CMD);
			channel->addClient(client, server);
		}
	}
	else
	{
		addFileLog("[+]Client: " + client->nick + " created channel: " + parameter, GREEN_CMD);
		server.channels.push_back(new Channel(parameter, client, server));
	}
}

void    Commands::execPrivmsg( const std::string & parameter, Client * client, Server & server )
{
	std::string to, msg;
	std::string::size_type space, colon;
	Channel *   channel;
	Client *    clientTo;

	//maybe we have to do a function for this if there are another commands that need it.
	space = parameter.find(" ");
	colon = parameter.find(":");
	if (space == std::string::npos || colon == std::string::npos)
	{
		// not sure if is this the valid response
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("PRIVMSG").Trailer("Not enough parameters").Send();
	}
	else
	{
		to = parameter.substr(0, space);
		msg = parameter.substr(colon + 1, parameter.size());
		channel = server.getChannelByName(to);
		if (!Channel::validName(to))
		{
			clientTo = server.getClientByNick(to);
			if (clientTo)
				Response::createMessage().From(*client).To(*clientTo).Command("PRIVMSG " + to).Trailer(msg).Send();
			else
				Response::createReply(ERR_NOSUCHNICK).From(server).To(*client).Command(to).Trailer("No such nick").Send();
		}
		else if (channel)
		{
			if (channel->isClient(client->nick))
				Response::createMessage().From(*client).Command("PRIVMSG " + to + " " + msg).Broadcast(channel->clients, false);
			else
				Response::createReply(ERR_CANNOTSENDTOCHAN).From(server).To(*client).Command(to).Trailer("Cannot send to channel").Send();
		}
		else
			Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command(to).Trailer("No such channel").Send();
	}
}

static std::vector<std::string> splitTopic(const std::string& str)
{
	std::vector<std::string> tokens;
	std::string::size_type pos = 0;
	std::string::size_type next = str.find(":");
	if (next != std::string::npos)
	{
		tokens.push_back(str.substr(pos, next - pos - 1));
		pos = next + 1;
		next = str.find(":", pos);
	}
	if (next == std::string::npos)
	{
		tokens.push_back(str.substr(pos));
	}
	return tokens;
}

void	Commands::execTopic( const std::string & parameter, Client * client, Server & server )
{
	std::vector<std::string>    tokens = splitTopic(parameter);
	Channel *channel = server.getChannelByName(tokens[0]);
	if (parameter[0] != '#' || !channel)
	{
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("NICK").Trailer("invalid channel").Send();
		addFileLog("[-]Client: " + client->nick + " tried to use topic on an invalid channel: " + parameter, RED_CMD);
		return ;
	}
	if (tokens.size() == 1)
	{
		if (channel->topic.empty())
			Response::createReply(RPL_NOTOPIC).From(server).To(*client).Command(channel->name).Trailer("No topic is set").Send();
		else
			Response::createReply(RPL_TOPIC).From(server).To(*client).Command(channel->name).Trailer(channel->topic).Send();
	}
	else
	{
		if (channel->isClient(client->nick))
		{
			if (channel->inviteOnly && client->channels[channel] != "@")
				Response::createReply(ERR_CHANOPRIVSNEEDED).From(server).To(*client).Command(channel->name).Trailer("You're not channel operator").Send();
			else
			{
				channel->topic = tokens[1];
				Response::createMessage().From(*client).Command("TOPIC " + channel->name + " :" + channel->topic).Broadcast(channel->clients, false);
				Response::createReply(RPL_TOPIC).From(server).To(*client).Command(channel->name).Trailer(channel->topic).Send();
				addFileLog("[+]Client: " + client->nick + " changed topic of channel: " + channel->name + " to: " + channel->topic, GREEN_CMD);
			}
		}
		else
		{
			Response::createReply(ERR_NOTONCHANNEL).From(server).To(*client).Command(channel->name).Trailer("You're not on that channel").Send();
			addFileLog("[-]Client: " + client->nick + " tried to change topic of channel: " + channel->name + " but is not on it.", RED_CMD);
		}
	}
}

void	Commands::execPart( const std::string & parameter, Client * client, Server & server )
{
	Channel *   channel;
	std::string name, reason;
	std::string::size_type space, colon;

	space = parameter.find(" ");
	colon = parameter.find(":");
	if (space == std::string::npos || !Channel::validName(parameter.substr(0, space)))
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("PART").Trailer("Not enough parameters").Send();
	else
	{
		name = parameter.substr(0, space);
		if (colon != std::string::npos)
			reason = parameter.substr(colon + 1, parameter.size());
		channel = server.getChannelByName(name);
		if (!channel)
			Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command(name).Trailer("No such channel").Send();
		else if (channel->isClient(client->nick))
		{
			Response::createMessage().From(*client).Command("PART " + name).Trailer(reason).Broadcast(client->channels, true);
			channel->delClient(client, server);
			client->channels.erase(channel);
		}
		else
			Response::createReply(ERR_NOTONCHANNEL).From(server).To(*client).Command(name).Trailer("You're not on that channel").Send();
	}
}

void	Commands::execKick( const std::string & parameter, Client * client, Server & server )
{
	std::string from, kick, reason;
	std::string::size_type firstSpace, secondSpace, colon;
	Channel *	channel;
	Client *	clientToKick;
	
	firstSpace = parameter.find(" ");
	secondSpace = parameter.find(" ", firstSpace + 1);
	std::cout << "first: " << firstSpace << " second " << secondSpace << std::endl;
	colon = parameter.find(":");
	reason = "Has been kicked by " + client->nick;
	if (firstSpace == std::string::npos || secondSpace == std::string::npos)
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("KICK").Trailer("Not enough parameters").Send();
	else
	{
		from = parameter.substr(0, firstSpace);
		kick = parameter.substr(firstSpace + 1, secondSpace - firstSpace - 1);
		std::cout << "To kick " + kick << std::endl;
		channel = server.getChannelByName(from);
		if (colon != std::string::npos)
			reason = parameter.substr(colon + 1, parameter.size());
		if (!Channel::validName(from))
			Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("KICK").Trailer("Not enough parameters").Send();
		else if (channel)
		{
			clientToKick = server.getClientByNick(kick);
			if (!channel->isClient(client->nick))
				Response::createReply(ERR_NOTONCHANNEL).From(server).To(*client).Command(channel->name).Trailer("You're not on that channel").Send();
			else if (client->channels[channel] != "@")
				Response::createReply(ERR_CHANOPRIVSNEEDED).From(server).To(*client).Command(from).Trailer("You're not channel operator").Send();
			else if (!clientToKick)
				Response::createReply(ERR_NOSUCHNICK).From(server).To(*client).Command(kick).Trailer("No such nick").Send();
			else
			{
				if (channel->isClient(kick))
				{
					Response::createMessage().From(*client).To(*clientToKick).Command("KICK " + from + " " + kick).Trailer(reason).Broadcast(channel->clients, true);
					channel->delClient(clientToKick, server);
					clientToKick->channels.erase(channel);
				}
				else
					Response::createReply(ERR_USERNOTINCHANNEL).From(server).To(*client).Command(kick + " " + channel->name).Trailer("They aren't on that channel").Send();
			}
		}
		else
			Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command(from).Trailer("No such channel").Send();
	}
}

void Commands::execInvite(const std::string& parameter, Client* client, Server& server) {
	if (parameter.find(' ') == std::string::npos) {
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("INVITE").Trailer("Not enough parameters").Send();
		addFileLog("[-]Client: " + client->nick + " tried to invite with not enough parameters", RED_CMD);
		return;
	}
	std::string to = parameter.substr(0, parameter.find(' '));
	std::string channel = parameter.substr(parameter.find(' ') + 1);
	Client* clientTo = server.getClientByNick(to);
	Channel* channelToInvite = server.getChannelByName(channel);
	if (!clientTo) {
		Response::createReply(ERR_NOSUCHNICK).From(server).To(*client).Command(to).Trailer("No such nick").Send();
		addFileLog("[-]Client: " + client->nick + " tried to invite " + to + " but is not a valid client", RED_CMD);
	} else if (!channelToInvite) {
		Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command(channel).Trailer("No such channel").Send();
		addFileLog("[-]Client: " + client->nick + " tried to invite " + to + " to an invalid channel: " + channel, RED_CMD);
	} else if (!channelToInvite->isClient(client->nick)) {
		Response::createReply(ERR_NOTONCHANNEL).From(server).To(*client).Command(channel).Trailer("You're not on that channel").Send();
		addFileLog("[-]Client: " + client->nick + " tried to invite " + to + " to channel: " + channel + " but is not on it", RED_CMD);
	} else if (channelToInvite->isClient(to)) {
		Response::createReply(ERR_USERONCHANNEL).From(server).To(*client).Command(to + " " + channel).Trailer("is already on channel").Send();
		addFileLog("[-]Client: " + client->nick + " tried to invite " + to + " to channel: " + channel + " but is already on it", RED_CMD);
	} else if (channelToInvite->inviteOnly && client->channels[channelToInvite] != "@") {
		Response::createReply(ERR_CHANOPRIVSNEEDED).From(server).To(*client).Command(channelToInvite->name).Trailer("You're not channel operator").Send();
		addFileLog("[-]Client: " + client->nick + " tried to invite " + to + " to channel: " + channel + " but is not channel operator", RED_CMD);
	} else if (channelToInvite->isInvite(clientTo)) {
		Response::createReply(ERR_USERONCHANNEL).From(server).To(*client).Command(to + " " + channel).Trailer("is already invited").Send();
		addFileLog("[-]Client: " + client->nick + " tried to invite " + to + " to channel: " + channel + " but is already invited", RED_CMD);
	} else {
		Response::createMessage().From(*client).To(*clientTo).Command("INVITE " + to + " " + channel).Trailer("Invite to " + channel).Send();
		Response::createReply(RPL_INVITING).From(server).To(*client).Command(to + " " + channel).Trailer("Inviting " + to + " to " + channel).Send();
		channelToInvite->inviteList.push_back(clientTo);
		addFileLog("[+]Client: " + client->nick + " invited " + to + " to channel: " + channel, GREEN_CMD);
	}
}
