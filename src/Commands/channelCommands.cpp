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
	if (parameter[0] != '#')
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
		tokens.push_back(str.substr(pos, next - pos));
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
	//Channel *   channel;
	std::cout << "mensaje: " << client->nick << " topic: " << parameter << std::endl;
	std::cout << "tokens size: " << tokens.size() << std::endl;
	std::cout << "tokens[0]: " << tokens[0] << std::endl;
	if (tokens.size() > 1)
		std::cout << "tokens[1]: " << tokens[1] << std::endl;
	if (parameter[0] != '#')
	{
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("NICK").Trailer("invalid channel to change topic").Send();
		addFileLog("[-]Client: " + client->nick + " tried to changed a topic on invalid channel: " + parameter, RED_CMD);
	}
	//channel = server.getChannelByName(parameter);
	
	(void) server;
}

// std::string	Server::_topic(Request request, int i)
// {
// 	if (!this->_clients[i]->getRegistered())
// 		return (_printMessage("451", this->_clients[i]->getNickName(), ":You have not registered"));
// 	if (request.args.size() == 0)
// 		return (_printMessage("461", this->_clients[i]->getNickName(), ":Not enough parameters"));
// 	if (request.args.size() == 1)
// 	{
// 		if (this->_allChannels.find(request.args[0])->second->getTopic().empty())
// 			return (_printMessage("331", this->_clients[i]->getNickName(), request.args[0] + " :No topic is set"));
// 		else
// 			return (_printMessage("332", this->_clients[i]->getNickName(), request.args[0] + " :" + this->_allChannels.find(request.args[0])->second->getTopic()));
// 	}
// 	std::map<std::string, Channel *>::iterator it = this->_allChannels.find(request.args[0]);
// 	if (it != this->_allChannels.end())
// 	{
// 		std::pair<Client *, int> user = it->second->findUserRole(i);
// 		if (user.second == 1)
// 		{
// 			it->second->setTopic(request.args[1]);
// 			std::string reply = "TOPIC " + it->second->getName() + ":" + request.args[1] + "\n";
// 			_sendToAllUsers(it->second, i, reply);
// 		}
// 		else if (user.second == -1  /* Not in channel */)
// 			return (_printMessage("442", this->_clients[i]->getNickName(), request.args[0] + " :You're not on that channel"));
// 		else
// 			return (_printMessage("482", this->_clients[i]->getNickName(), request.args[0] + " :You're not channel operator"));
// 	}
// 	return ("");
// }