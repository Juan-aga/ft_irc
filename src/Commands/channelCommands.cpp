#include "Commands.hpp"
#include "Server.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/unistd.h>

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
	std::string::size_type space;
	std::string	password, channelName;

	//maybe we have to do a function for this if there are another commands that need it.
	password = "";
	space = parameter.find(" ");
	channelName = parameter.substr(0, space);
	channel = server.getChannelByName(channelName);
	if (space != std::string::npos)
		password = parameter.substr(space + 1, parameter.size());
	if (parameter.size() == 1 && parameter[0] == '0')
	{
		for (std::map< Channel *, std::string >::iterator chan = client->channels.begin(); client->channels.size() > 0; chan = client->channels.begin())
			execPart(chan->first->name + " " , client, server);
	}
	else if (!Channel::validName(channelName))
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("JOIN").Trailer("Not enough parameters").Send("[-]Client: " + client->nick + " tried to join an invalid channel: " + channelName, RED_CMD);
	else if (channel)
	{
		if (channel->isClient(client->nick))
			Response::createReply(ERR_USERONCHANNEL).From(server).To(*client).Command(channelName).Trailer("is already on channel").Send("[!]Client: " + client->nick + " is already a member of channel: " + channelName, YELLOW_CMD);
		else if (channel->inviteOnly && !channel->isInvite(client))
			Response::createReply(ERR_INVITEONLYCHAN).From(server).To(*client).Command(channelName).Trailer("Cannot join channel (Invite only)").Send("[-]Client: " + client->nick + " tried to join channel: " + channelName + " but is invite only", RED_CMD);
		else if (channel->clientLimit > 0 && static_cast<int>(channel->clients.size()) >= channel->clientLimit)
			Response::createReply(ERR_CHANNELISFULL).From(server).To(*client).Command(channelName).Trailer("Cannot join channel (Channel is full)").Send("[-]Client: " + client->nick + " tried to join channel: " + channelName + " but is full", RED_CMD);
		else
			channel->addClient(client, server, password);
	}
	else
		server.channels.push_back(new Channel(channelName, client, server));
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
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("PRIVMSG").Trailer("Not enough parameters").Send();
	else
	{
		to = parameter.substr(0, space);
		msg = parameter.substr(colon + 1, parameter.size());
		channel = server.getChannelByName(to);
		if (!Channel::validName(to))
		{
			clientTo = server.getClientByNick(to);
			if (clientTo && clientTo->nick != "bot") // if client exists and is not bot
				Response::createMessage().From(*client).To(*clientTo).Command("PRIVMSG " + to).Trailer(msg).Send();
			else if (clientTo) // if client exists and is bot
				Response::createMessage().From(*clientTo).To(*client).Command("PRIVMSG " + client->nick).Trailer(server.bot(msg)).Send("[+]Client: " + client->nick + " used bot command: " + msg, GREEN_CMD);
			else // if client doesn't exist
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

static std::vector<std::string> splitString(const std::string& input, char delimiter) {
	std::vector<std::string> parts;

	size_t pos = 0;
	std::string token;
	std::string str = input;

	pos = str.find(delimiter);
	while (pos != std::string::npos) {
		token = str.substr(0, pos);
		parts.push_back(token);
		str.erase(0, pos + 1);
		pos = str.find(delimiter);
	}
	if (!str.empty())
		parts.push_back(str.substr());

	return parts;
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
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("NICK").Trailer("invalid channel").Send("[-]Client: " + client->nick + " tried to use topic on an invalid channel: " + parameter, RED_CMD);
		return ;
	}
	if (tokens.size() == 1)
	{
		if (channel->topic.empty())
			Response::createReply(RPL_NOTOPIC).From(server).To(*client).Command(channel->name).Trailer("No topic is set").Send("[+]Client: " + client->nick + " get topic of channel: " + channel->name + " but is not set", GREEN_CMD);
		else
			Response::createReply(RPL_TOPIC).From(server).To(*client).Command(channel->name).Trailer(channel->topic).Send("[+]Client: " + client->nick + " get topic of channel: " + channel->name + " to: " + channel->topic, GREEN_CMD);
	}
	else
	{
		if (channel->isClient(client->nick))
		{
			if (channel->opTopic && client->channels[channel] != "@")
				Response::createReply(ERR_CHANOPRIVSNEEDED).From(server).To(*client).Command(channel->name).Trailer("You're not channel operator").Send("[-]Client: " + client->nick + " tried to change topic of channel: " + channel->name + " but is not operator", RED_CMD);
			else
			{
				channel->topic = tokens[1];
				Response::createMessage().From(*client).Command("TOPIC " + channel->name + " :" + channel->topic).Broadcast(channel->clients, false);
				Response::createReply(RPL_TOPIC).From(server).To(*client).Command(channel->name).Trailer(channel->topic).Send("[+]Client: " + client->nick + " changed topic of channel: " + channel->name + " to: " + channel->topic, GREEN_CMD);
			}
		}
		else
			Response::createReply(ERR_NOTONCHANNEL).From(server).To(*client).Command(channel->name).Trailer("You're not on that channel").Send("[-]Client: " + client->nick + " tried to change topic of channel: " + channel->name + " but is not on it.", RED_CMD);
	}
}


void		Commands::execMode(const std::string & parameter, Client * client, Server & server)
{
	std::vector<std::string> parameters;
	Channel *channel;
	Client *clientTo;
	// check if client is op
	parameters.push_back(parameter);
	if (parameter.empty())
	{
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Not enough parameters").Send("[-]Client: " + client->nick + " tried to use mode with not enough parameters", RED_CMD);
		return ;
	}
	parameters = splitString(parameter, ' ');
	channel = server.getChannelByName(parameters[0]);
	if (parameters[0][0] == '#' && parameters.size() > 1) // there is <cahnnel> <modestring>
	{
		// if channel doesn't exist send a ERR_NOSUCHCHANNEL
		if (client->channels[channel] != "@")
		{
			Response::createReply(ERR_CHANOPRIVSNEEDED).From(server).To(*client).Command(parameters[0]).Trailer("You're not channel operator").Send("[-]Client: " + client->nick + " tried to use mode on channel: " + parameters[0] + " but is not operator", RED_CMD);
			return ;
		}
		else if (!channel->validName(parameters[0]) || !channel)
			Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command("MODE").Trailer("Not such channel").Send("[-]Client: " + client->nick + " tried to use mode on invalid channel: " + parameters[0], RED_CMD);
		// else check the flags if exists
		else if (parameter[1] && (parameters[1].size() == 2 && parameters[1][0] == '+'))
		{
			if (parameters[1] == "+i")
			{
				if (!channel->inviteOnly)
				{
					channel->inviteOnly = true;
					Response::createMessage().From(*client).Command("MODE " + server.getChannelByName(parameters[0])->name + " +i").Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to invite only", GREEN_CMD);
				}
			}
			else if (parameters[1] == "+t")
			{
				if (!channel->opTopic)
				{
					channel->opTopic = true;
					Response::createMessage().From(*client).Command("MODE " + channel->name + " +t").Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to op topic", GREEN_CMD);
				}
			}
			else if (parameters[1] == "+k")
			{
				if (parameters[2] != "")
				{
					if (channel->needPass)
						Response::createReply(ERR_KEYSET).From(server).To(*client).Command("MODE").Trailer("Channel key already set").Send("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to password protected but is already set", RED_CMD);
					else
					{
						channel->password = parameters[2];
						channel->needPass = true;
						Response::createMessage().From(*client).Command("MODE " + channel->name + " +k " + parameters[2]).Broadcast(channel->clients, true);
						addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to password protected", GREEN_CMD);
					}
				}
				else
					Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to password protected but didn't provide password", RED_CMD);
			}
			else if (parameters[1] == "+l")
			{
				if (parameters[2] == "")
					Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to limit but didn't provide limit", RED_CMD);
				else
				{
					if (!channel->isLimited)
					{
						channel->clientLimit = std::atoi(parameters[2].c_str());
						channel->isLimited = true;
						Response::createMessage().From(*client).Command("MODE " + channel->name + " +l " + parameters[2]).Broadcast(channel->clients, true);
						addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to limit", GREEN_CMD);
					}
				}
			}
			else if (parameters[1] == "+o" && parameters.size() > 2)
			{
				if (parameters[2] == "")
					Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to limit but didn't provide limit", RED_CMD);
				else
				{
					clientTo = server.getClientByNick(parameters[2]);
					if (clientTo != NULL && channel->isClient(clientTo->nick) && clientTo->channels[channel] != "@")
					{
						clientTo->channels[channel] = "@";
						Response::createMessage().From(*client).Command("MODE " + channel->name + " +o " + clientTo->nick).Broadcast(channel->clients, true);
						addFileLog("[+]Client: " + client->nick + " set client: " + clientTo->nick + " to operator in channel: " + channel->name, GREEN_CMD);
					}
					else
						Response::createReply(ERR_USERSDONTMATCH).From(server).To(*client).Command("MODE").Trailer("Users don't match").Send("[-]Client: " + client->nick + " tried to set client: " + parameters[2] + " to operator in channel: " + channel->name + " but client doesn't exist or is already operator", RED_CMD);
				}

			}
			else
				Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to something but didn't provide enough parameters", RED_CMD);
		}
		else if (parameter[1] && (parameters[1].size() == 2 && parameters[1][0] == '-'))
		{
			if (parameters[1] == "-i")
			{
				if (channel->inviteOnly)
				{
					channel->inviteOnly = false;
					Response::createMessage().From(*client).Command("MODE " + channel->name + " -i").Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to no invite only", GREEN_CMD);
				}
			}
			else if (parameters[1] == "-t")
			{
				if (channel->opTopic)
				{
					channel->opTopic = false;
					Response::createMessage().From(*client).Command("MODE " + channel->name + " -t").Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to no op topic", GREEN_CMD);
				}
			}
			else if (parameters[1] == "-k")
			{
				if (channel->needPass)
				{
					channel->password = "";
					channel->needPass = false;
					Response::createMessage().From(*client).Command("MODE " + channel->name + " -k").Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to no password protected", GREEN_CMD);
				}
			}
			else if (parameters[1] == "-l")
			{
				if (channel->isLimited)
				{
					channel->clientLimit = 0;
					channel->isLimited = false;
					Response::createMessage().From(*client).Command("MODE " + channel->name + " -l").Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to no limit", GREEN_CMD);
				}
			}
			else if (parameters[1] == "-o" && parameters.size() > 2)
			{
				clientTo = server.getClientByNick(parameters[2]);
				if (clientTo != NULL && channel->isClient(clientTo->nick) && clientTo->channels[channel] == "@" && clientTo->nick != client->nick)
				{
					clientTo->channels[channel] = "+";
					Response::createMessage().From(*client).Command("MODE " + channel->name + " -o " + clientTo->nick).Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " set client: " + clientTo->nick + " to no operator in channel: " + channel->name, GREEN_CMD);
				}
				else
					Response::createReply(ERR_USERSDONTMATCH).From(server).To(*client).Command("MODE").Trailer("Users don't match").Send("[-]Client: " + client->nick + " tried to unset client: " + parameters[2] + " to operator in channel: " + channel->name + " but client doesn't exist or is not operator", RED_CMD);
			}
			else
				Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to something but didn't provide enough parameters", RED_CMD);
		}
	}
	else if (parameters.size() == 1)
		Response::createReply(RPL_CHANNELMODEIS).From(server).To(*client).Command(channel->name).Trailer(channel->getMode()).Send("[+]Client: " + client->nick + " get mode of channel: " + channel->name + " to: " + channel->getMode(), GREEN_CMD);
}

void Commands::execInvite(const std::string& parameter, Client* client, Server& server) {
	if (parameter.find(' ') == std::string::npos) {
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("INVITE").Trailer("Not enough parameters").Send("[-]Client: " + client->nick + " tried to invite with not enough parameters", RED_CMD);
		return;
	}
	std::string to = parameter.substr(0, parameter.find(' '));
	std::string channel = parameter.substr(parameter.find(' ') + 1);
	Client* clientTo = server.getClientByNick(to);
	Channel* channelToInvite = server.getChannelByName(channel);
	if (!clientTo) // if client doesn't exist
		Response::createReply(ERR_NOSUCHNICK).From(server).To(*client).Command(to).Trailer("No such nick").Send("[-]Client: " + client->nick + " tried to invite " + to + " but is not a valid client", RED_CMD);
	else if (!channelToInvite) // if channel doesn't exist
		Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command(channel).Trailer("No such channel").Send("[-]Client: " + client->nick + " tried to invite " + to + " to an invalid channel: " + channel, RED_CMD);
	else if (!channelToInvite->isClient(client->nick)) // if client is not on channel
		Response::createReply(ERR_NOTONCHANNEL).From(server).To(*client).Command(channel).Trailer("You're not on that channel").Send("[-]Client: " + client->nick + " tried to invite " + to + " to channel: " + channel + " but is not on it", RED_CMD);
	else if (channelToInvite->isClient(to)) // if client is already on channel
		Response::createReply(ERR_USERONCHANNEL).From(server).To(*client).Command(to + " " + channel).Trailer("is already on channel").Send("[-]Client: " + client->nick + " tried to invite " + to + " to channel: " + channel + " but is already on it", RED_CMD);
	else if (channelToInvite->inviteOnly && client->channels[channelToInvite] != "@") // if channel is invite only
		Response::createReply(ERR_CHANOPRIVSNEEDED).From(server).To(*client).Command(channelToInvite->name).Trailer("You're not channel operator").Send("[-]Client: " + client->nick + " tried to invite " + to + " to channel: " + channel + " but is not channel operator", RED_CMD);
	else if (channelToInvite->isInvite(clientTo)) // if client is already invited
		Response::createReply(ERR_USERONCHANNEL).From(server).To(*client).Command(to + " " + channel).Trailer("is already invited").Send("[-]Client: " + client->nick + " tried to invite " + to + " to channel: " + channel + " but is already invited", RED_CMD);
	else {
		Response::createMessage().From(*client).To(*clientTo).Command("INVITE " + to + " " + channel).Trailer("Invite to " + channel).Send();
		Response::createReply(RPL_INVITING).From(server).To(*client).Command(to + " " + channel).Trailer("Inviting " + to + " to " + channel).Send("[+]Client: " + client->nick + " invited " + to + " to channel: " + channel, GREEN_CMD);
		channelToInvite->inviteList.push_back(clientTo);
	} // if client is not on channel
}

void	Commands::execPart( const std::string & parameter, Client * client, Server & server )
{
	Channel *   channel;
	std::string name, reason;
	std::string::size_type space, colon;

	space = parameter.find(" ");
	colon = parameter.find(":");
	if (space == std::string::npos || !Channel::validName(parameter.substr(0, space)))

		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("PART").Trailer("Not enough parameters").Send("[-]Client: " + client->nick + " tried to part with not enough parameters", RED_CMD);
	else
	{
		name = parameter.substr(0, space);
		if (colon != std::string::npos)
			reason = parameter.substr(colon + 1, parameter.size());
		channel = server.getChannelByName(name);
		if (!channel)
			Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command(name).Trailer("No such channel").Send("[-]Client: " + client->nick + " tried to part from invalid channel: " + name, RED_CMD);
		else if (channel->isClient(client->nick))
		{
			Response::createMessage().From(*client).Command("PART " + name).Trailer(reason).Broadcast(channel->clients, true);
			addFileLog("[+]Client: " + client->nick + " parted from channel: " + name, GREEN_CMD);
			channel->delClient(client, server);
			client->channels.erase(channel);
		}
		else
			Response::createReply(ERR_NOTONCHANNEL).From(server).To(*client).Command(name).Trailer("You're not on that channel").Send("[-]Client: " + client->nick + " tried to part from channel: " + name + " but is not on it", RED_CMD);
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
	colon = parameter.find(":");
	reason = "Has been kicked by " + client->nick;
	if (firstSpace == std::string::npos || secondSpace == std::string::npos)
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("KICK").Trailer("Not enough parameters").Send("[-]Client: " + client->nick + " tried to kick with not enough parameters", RED_CMD);
	else
	{
		from = parameter.substr(0, firstSpace);
		kick = parameter.substr(firstSpace + 1, secondSpace - firstSpace - 1);
		channel = server.getChannelByName(from);
		if (colon != std::string::npos) // if there is a reason
			reason = parameter.substr(colon + 1, parameter.size());
		if (!Channel::validName(from)) // if channel doesn't exist
			Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("KICK").Trailer("Not enough parameters").Send("[-]Client: " + client->nick + " tried to kick with not enough parameters", RED_CMD);
		else if (channel)
		{
			clientToKick = server.getClientByNick(kick);
			if (!channel->isClient(client->nick)) // if client is not on channel
				Response::createReply(ERR_NOTONCHANNEL).From(server).To(*client).Command(channel->name).Trailer("You're not on that channel").Send("[-]Client: " + client->nick + " tried to kick from channel: " + channel->name + " but is not on it", RED_CMD);
			else if (client->channels[channel] != "@") // if client is not op
				Response::createReply(ERR_CHANOPRIVSNEEDED).From(server).To(*client).Command(from).Trailer("You're not channel operator").Send("[-]Client: " + client->nick + " tried to kick from channel: " + channel->name + " but is not operator", RED_CMD);
			else if (!clientToKick) // if client to kick doesn't exist
				Response::createReply(ERR_NOSUCHNICK).From(server).To(*client).Command(kick).Trailer("No such nick").Send("[-]Client: " + client->nick + " tried to kick from channel: " + channel->name + " but is not a valid client", RED_CMD);
			else
			{
				if (channel->isClient(kick)) // if client to kick is on channel
				{
					Response::createMessage().From(*client).To(*clientToKick).Command("KICK " + from + " " + kick).Trailer(reason).Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " kicked " + kick + " from channel: " + channel->name, GREEN_CMD);
					channel->delClient(clientToKick, server);
					clientToKick->channels.erase(channel);
				}
				else
					Response::createReply(ERR_USERNOTINCHANNEL).From(server).To(*client).Command(kick + " " + channel->name).Trailer("They aren't on that channel").Send("[-]Client: " + client->nick + " tried to kick from channel: " + channel->name + " but is not on it", RED_CMD);
			}
		}
		else
			Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command(from).Trailer("No such channel").Send("[-]Client: " + client->nick + " tried to kick from invalid channel: " + from, RED_CMD);
	}
}
