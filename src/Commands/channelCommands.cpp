#include "Commands.hpp"
#include "Server.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <sys/stdio.h>
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
	std::string	password = "";

	//maybe we have to do a function for this if there are another commands that need it.
	space = parameter.find(" ");
	channel = server.getChannelByName(parameter.substr(0, space));
	if (space != std::string::npos)
		password = parameter.substr(space + 1, parameter.size());
	// If parameter is "0", the client leave all channels. execute PART for every channel.
	if (parameter.size() == 1 && parameter[0] == '0')
	{
		for (std::map< Channel *, std::string >::iterator chan = client->channels.begin(); client->channels.size() > 0; chan = client->channels.begin())
			execPart(chan->first->name + " " , client, server);
	}
	else if (!Channel::validName(parameter.substr(0, space)))
	{
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("JOIN").Trailer("Not enough parameters").Send();
		addFileLog("[-]Client: " + client->nick + " tried to join an invalid channel: " + parameter, RED_CMD);
	}
	else if (channel)
	{
		if (channel->isClient(client->nick))
		{
			Response::createReply(ERR_USERONCHANNEL).From(server).To(*client).Command(parameter).Trailer("is already on channel").Send();
			addFileLog("[!]Client: " + client->nick + " is already a member of channel: " + parameter, YELLOW_CMD);
		}
		else if (channel->inviteOnly && !channel->isInvite(client))
		{
			Response::createReply(ERR_INVITEONLYCHAN).From(server).To(*client).Command(parameter).Trailer("Cannot join channel (Invite only)").Send();
			addFileLog("[-]Client: " + client->nick + " tried to join channel: " + parameter + " but is invite only", RED_CMD);
		}
		else if (channel->clientLimit > 0 && static_cast<int>(channel->clients.size()) >= channel->clientLimit)
		{
			Response::createReply(ERR_CHANNELISFULL).From(server).To(*client).Command(parameter).Trailer("Cannot join channel (Channel is full)").Send();
			addFileLog("[-]Client: " + client->nick + " tried to join channel: " + parameter + " but is full", RED_CMD);
		}
		else
		{
			if (channel->addClient(client, server, password))
				addFileLog("[+]Client: " + client->nick + " joined channel: " + parameter, GREEN_CMD);
			else
				std::cout << "ERROR, pasword not match.\n";
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
			if (channel->opTopic && client->channels[channel] != "@")
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


void		Commands::execMode(const std::string & parameter, Client * client, Server & server)
{
	std::vector<std::string> parameters;
	Channel *channel;
	Client *clientTo;
	std::cout << "parameter: " << parameter << std::endl;
	// check if client is op
	parameters.push_back(parameter);
	if (parameter.empty())
	{
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Not enough parameters").Send();
		return ;
	}
	parameters = splitString(parameter, ' ');
	channel = server.getChannelByName(parameters[0]);
	if (parameters[0][0] == '#' && parameters.size() > 1) // there is <cahnnel> <modestring>
	{
		// if channel doesn't exist send a ERR_NOSUCHCHANNEL
		if (client->channels[channel] != "@")
		{
			Response::createReply(ERR_CHANOPRIVSNEEDED).From(server).To(*client).Command(parameters[0]).Trailer("You're not channel operator").Send();
			addFileLog("[-]Client: " + client->nick + " tried to use mode on channel: " + parameters[0] + " but is not operator", RED_CMD);
			return ;
		}
		else if (!channel->validName(parameters[0]) || !channel)
		{
			Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command("MODE").Trailer("Not such channel").Send();
			addFileLog("[-]Client: " + client->nick + " tried to use mode on invalid channel: " + parameters[0], RED_CMD);
		}
		// else check the flags if exists
		else if (parameter[1] && (parameters[1].size() == 2 && parameters[1][0] == '+'))
		{
			if (parameters[1] == "+i")
			{
				channel->inviteOnly = true;
				Response::createMessage().From(*client).Command("MODE " + server.getChannelByName(parameters[0])->name + " +i").Broadcast(channel->clients, true);
				addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to invite only", GREEN_CMD);
			}
			else if (parameters[1] == "+t")
			{
				channel->opTopic = true;
				Response::createMessage().From(*client).Command("MODE " + channel->name + " +t").Broadcast(channel->clients, true);
				addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to op topic", GREEN_CMD);
			}
			else if (parameters[1] == "+k")
			{
				if (parameters[2] != "")
				{
					channel->password = parameters[2];
					channel->needPass = true;
					Response::createMessage().From(*client).Command("MODE " + channel->name + " +k " + channel->password).Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to password protected", GREEN_CMD);
				}
				else
				{
					Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send();
					addFileLog("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to password protected but didn't provide password", RED_CMD);
				}
			}
			else if (parameters[1] == "+l")
			{
				if (parameters[2] == "")
				{
					Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send();
					addFileLog("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to limit but didn't provide limit", RED_CMD);
				}
				else
				{
					channel->clientLimit = std::atoi(parameters[2].c_str());
					channel->isLimited = true;
					Response::createMessage().From(*client).Command("MODE " + channel->name + " +l " + parameters[2]).Broadcast(channel->clients, true);
					addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to limit: " + parameters[2], GREEN_CMD);
				}
			}
			else if (parameters[1] == "+o")
			{
				if (parameters[2] == "")
				{
					Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send();
					addFileLog("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to limit but didn't provide limit", RED_CMD);
				}
				else
				{
					clientTo = server.getClientByNick(parameters[2]);
					if (clientTo && clientTo->channels[channel] != "@")
					{
						clientTo->channels[channel] = "@";
						std::cout << "clientTo->channels[channel] = " << clientTo->channels[channel] << std::endl;
						Response::createMessage().From(*client).Command("MODE " + channel->name + " +o " + clientTo->nick).Broadcast(channel->clients, true);
						addFileLog("[+]Client: " + client->nick + " set client: " + clientTo->nick + " to operator in channel: " + channel->name, GREEN_CMD);
					}
					else
					{
						Response::createReply(ERR_USERSDONTMATCH).From(server).To(*client).Command("MODE").Trailer("Users don't match").Send();
						addFileLog("[-]Client: " + client->nick + " tried to set client: " + clientTo->nick + " to operator in channel: " + channel->name + " but client doesn't exist or is already operator", RED_CMD);
					}
				}

			}
			else
				std::cout << "channel MODE error" << std::endl;
		}
		else if (parameter[1] && (parameters[1].size() == 2 && parameters[1][0] == '-'))
		{
			if (parameters[1] == "-i")
			{
				channel->inviteOnly = false;
				Response::createMessage().From(*client).Command("MODE " + channel->name + " -i").Broadcast(channel->clients, true);
				addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to invite only", GREEN_CMD);
			}
			else if (parameters[1] == "-t")
			{
				channel->opTopic = false;
				Response::createMessage().From(*client).Command("MODE " + channel->name + " -t").Broadcast(channel->clients, true);
				addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to op topic", GREEN_CMD);
			}
			else if (parameters[1] == "-k")
			{
				channel->password = "";
				channel->needPass = false;
				Response::createMessage().From(*client).Command("MODE " + channel->name + " -k").Broadcast(channel->clients, true);
				addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to password protected", GREEN_CMD);
			}
			else if (parameters[1] == "-l")
			{
				channel->clientLimit = 0;
				channel->isLimited = false;
				Response::createMessage().From(*client).Command("MODE " + channel->name + " -l").Broadcast(channel->clients, true);
				addFileLog("[+]Client: " + client->nick + " set channel: " + channel->name + " to no limit", GREEN_CMD);
			}
			else if (parameters[1] == "-o")
			{
				clientTo = server.getClientByNick(parameters[2]);
				clientTo->channels[channel] = "";
				std::cout << "clientTo->channels[channel] = " << clientTo->channels[channel] << std::endl;
				Response::createMessage().From(*client).Command("MODE " + channel->name + " -o " + clientTo->nick).Broadcast(channel->clients, true);
				addFileLog("[+]Client: " + client->nick + " set client: " + clientTo->nick + " to no operator in channel: " + channel->name, GREEN_CMD);
			}
			else
			{
				Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send();
				addFileLog("[-]Client: " + client->nick + " tried to set channel: " + channel->name + " to something but didn't provide enough parameters", RED_CMD);
			}
		}
	}
	else if (parameters.size() == 1)
	{
		Response::createReply(RPL_CHANNELMODEIS).From(server).To(*client).Command(channel->name).Trailer("+" + channel->getMode()).Send();
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
			Response::createMessage().From(*client).Command("PART " + name).Trailer(reason).Broadcast(channel->clients, true);
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
