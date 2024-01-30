#include "Commands.hpp"
#include "Server.hpp"

void        Commands::execJoin( const std::string & parameter, Client & client, Server & server )
{
	Channel *   channel;
	
	channel = server.getChannelByName(parameter);
	if (parameter[0] != '#')
	{
		addFileLog("[-]Client: " + client.nick + " tried to join an invalid channel: " + parameter, RED_CMD);
		//if channel's name is not valid send a respionse	
	}
	else if (channel)
	{
		if (channel->isClient(client.nick))
			addFileLog("[!]Client: " + client.nick + " is already a member of channel: " + parameter, YELLOW_CMD);
		//we have to check permmisions
		else
		{
			addFileLog("[+]Client: " + client.nick + " joined channel: " + parameter, GREEN_CMD);
			channel->addClient(&client, server);
		}
	}
	else
	{
		addFileLog("[+]Client: " + client.nick + " created channel: " + parameter, GREEN_CMD);
		server.channels.push_back(new Channel(parameter, & client, server));
	}
}

void    Commands::execPrivmsg( const std::string & parameter, Client & client, Server & server )
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
			if (channel->isClient(client.nick))
				Response::createMessage().From(client).Command("PRIVMSG " + to + " " + msg).Broadcast(channel->clients, false);
			else
			{
				//the client is not on the channel
				std::cout << client.nick << " is not in " << channel->name << " can't send messages.\n";
			}
		}
		//check if is a client. it will be better wen implemented the checker for valid names for nick and channels
		// if start with # it's a channel, else a user.
		else
		{
			Client *    clientTo;
			
			clientTo = server.getClientByNick(to);
			if (clientTo)
				Response::createMessage().From(client).To(*clientTo).Command("PRIVMSG " + to).Trailer(msg).Send();
			else
			{
				//the client don't exist
				// check if we have to send a RPL
				std::cout << to << " client don't exist.\n";
			}
		}
	}
}