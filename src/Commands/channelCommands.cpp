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

//split
static std::vector<std::string> splitString(const std::string& input, char delimiter) {
	std::vector<std::string> parts;
	
	size_t pos = 0;
	std::string token;
	std::string str = input;  
	
	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		parts.push_back(token);
		str.erase(0, pos + 1);
	}
	parts.push_back(str);
	
	return parts;
}

void		Commands::execMode(const std::string & parameter, Client & client, Server & server)
{
	std::vector<std::string> parameters;
	parameters.push_back(parameter);
	(void)client;
	(void)server;
	// (void)parameter;
	
	parameters = splitString(parameter, ' ');
	if (parameters[0] == client.nick)
	{
		std::cout << "server MODE" << std::endl;
		//server MODE
	}
	else if (parameters[0][0] == '#') //
	{
		std::cout << "channel MODE" << std::endl;
		if ((parameters[1].size() == 2 && parameters[1][0] == '+'))
		{
			if (parameters[1] == "+i")
				std::cout << "channel MODE invite only" << std::endl;
			else if (parameters[1] == "+t")
				std::cout << "channel MODE protected topic" << std::endl;
			else if (parameters[1] == "+k")
				std::cout << "channel MODE key channel" << std::endl;
			else if (parameters[1] == "+l")
				std::cout << "channel MODE limit client" << std::endl;
			else
				std::cout << "channel MODE error" << std::endl;
			
		}
			std::cout << "channel MODE parameter" << parameters[1] << std::endl;
		//chanel MODE
	}
	else
		std::cout << "void MODE" << std::endl;
	//std::cout << "[!]/MODE param1: " << parameters[1] << std::endl;
	//delete [] char_array;
}

void        Commands::execJoin( const std::string & parameter, Client & client, Server & server )
{
	Channel *   channel;
	
    // If parameter is "0", the client leave all channels. execute PART for every channel.
	channel = server.getChannelByName(parameter);
	if (parameter[0] != '#')
	{
        Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(client).Command("NICK").Trailer("Not enough parameters").Send();
		addFileLog("[-]Client: " + client.nick + " tried to join an invalid channel: " + parameter, RED_CMD);
	}
	else if (channel)
	{
		if (channel->isClient(client.nick))
			addFileLog("[!]Client: " + client.nick + " is already a member of channel: " + parameter, YELLOW_CMD);
		//we have to check permmisions
        // send ERR_INVITEONLYCHAN (473) or ERR_BANNEDFROMCHAN (474)
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