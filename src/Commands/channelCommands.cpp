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
	
	// If parameter is "0", the client leave all channels. execute PART for every channel.
	channel = server.getChannelByName(parameter);
	if (parameter == "" || parameter[0] != '#' || !parameter[1])
	{
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("JOIN").Trailer("Not enough parameters").Send();
		addFileLog("[-]Client: " + client->nick + " tried to join an invalid channel: " + parameter, RED_CMD);
	}
	else if (channel)
	{
		if (channel->isClient(client->nick))
			addFileLog("[!]Client: " + client->nick + " is already a member of channel: " + parameter, YELLOW_CMD);
		//we have to check permmisions
		// send ERR_INVITEONLYCHAN (473) or ERR_BANNEDFROMCHAN (474)
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

	//maybe we have to do a function for this if there are another commands that need it.
	space = parameter.find(" ");
	colon = parameter.find(":");
	if (space == std::string::npos || colon == std::string::npos)
	{
		std::cout << "Error parsing PRIVMSG.\n";
	}
	else
	{
		Channel *   channel;
		
		to = parameter.substr(0, space);
		msg = parameter.substr(colon + 1, parameter.size());
		channel = server.getChannelByName(to);

		if (channel)
		{
			if (channel->isClient(client->nick))
				Response::createMessage().From(*client).Command("PRIVMSG " + to + " " + msg).Broadcast(channel->clients, false);
			else
			{
				//the client is not on the channel
				std::cout << client->nick << " is not in " << channel->name << " can't send messages.\n";
			}
		}
		//check if is a client. it will be better wen implemented the checker for valid names for nick and channels
		// if start with # it's a channel, else a user.
		else
		{
			Client *    clientTo;
			
			clientTo = server.getClientByNick(to);
			if (clientTo)
				Response::createMessage().From(*client).To(*clientTo).Command("PRIVMSG " + to).Trailer(msg).Send();
			else
			{
				//the client don't exist
				// check if we have to send a RPL
				std::cout << to << " client don't exist.\n";
			}
		}
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
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(client).Command("NICK").Trailer("invalid channel").Send();
		addFileLog("[-]Client: " + client.nick + " tried to use topic on an invalid channel: " + parameter, RED_CMD);
		return ;
	}
	if (tokens.size() == 1)
	{
		if (channel->topic.empty())
			Response::createReply(RPL_NOTOPIC).From(server).To(client).Command(channel->name).Trailer("No topic is set").Send();
		else
			Response::createReply(RPL_TOPIC).From(server).To(client).Command(channel->name).Trailer(channel->topic).Send();
	}
	else
	{
		if (channel->isClient(client.nick))
		{
			//add here the check for the permissions to change the topic if you are the operator
			channel->topic = tokens[1];
			Response::createMessage().From(client).Command("TOPIC " + channel->name + " :" + channel->topic).Broadcast(channel->clients, false);
			Response::createReply(RPL_TOPIC).From(server).To(client).Command(channel->name).Trailer(channel->topic).Send();
			addFileLog("[+]Client: " + client.nick + " changed topic of channel: " + channel->name + " to: " + channel->topic, GREEN_CMD);
		}
		else
		{
			Response::createReply(ERR_NOTONCHANNEL).From(server).To(client).Command(channel->name).Trailer("You're not on that channel").Send();
			addFileLog("[-]Client: " + client.nick + " tried to change topic of channel: " + channel->name + " but is not on it.", RED_CMD);
		}
	}
}