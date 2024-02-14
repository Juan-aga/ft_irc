#include "Commands.hpp"
#include "Server.hpp"

void        Commands::execCap( const std::string & parameter, Client * client, Server & server )
{
	(void)parameter;
	(void)client;
	(void)server;
	addFileLog("[!]Client from ip: " + client->ip + " trying CAP: " + parameter + ". Not supported.", YELLOW_CMD);
}

void		Commands::execWho( const std::string & parameter, Client * client, Server & server )
{
	(void)parameter;
	(void)client;
	(void)server;
	addFileLog("[!]Client from ip: " + client->ip + " trying WHO: " + parameter + ". Not supported.", YELLOW_CMD);
}

void        Commands::execPass( const std::string & parameter, Client * client, Server & server )
{
	if (parameter.empty()) //not enough parameters
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("PASS").Trailer("Not enough parameters").Send("[-]Client from ip: " + client->ip + " failed to connect (Not enough parameters).", RED_CMD);
	else if (client->status >= CONNECTED) //already connected
		Response::createReply(ERR_ALREADYREGISTERED).From(server).To(*client).Trailer("You may not reregister").Send("[-]Client: " + client->nick + " already connected (You may not reregister).", RED_CMD);
	else if (parameter == server.getPassword()) //password correct
		client->status = AUTH;
	else //password incorrect
	{
		Response::createReply(ERR_PASSWDMISMATCH).From(server).To(*client).Trailer("Password incorrect").Send("[-]Client from ip: " + client->ip + " failed to connect (incorrect password).", RED_CMD);
		client->status = DISCONECT;
	}
}

void Commands::execNick( const std::string & parameter, Client * client, Server & server )
{
	Client * check;

	if (parameter.empty()) // not enough parameters
	{
		if (client->nick == "")
			Response::createReply(ERR_NONICKNAMEGIVEN).From(server).To(*client).Trailer("No nickname given").Send("[-]Client from ip: " + client->ip + " failed to execute NICK (No nickname given).", RED_CMD);
		else
			Response::createReply(ERR_NONICKNAMEGIVEN).From(server).To(*client).Trailer("No nickname given").Send("[-]Client: " + client->nick + " failed to execute NICK (No nickname given).", RED_CMD);
	}
	else if (!parseNick(parameter)) //check if the nickname is valid
	{
		if (client->nick == "")
			Response::createReply(ERR_ERRONEUSNICKNAME).From(server).To(*client).Command(parameter).Trailer("Erroneus nickname").Send("[-]Client from ip: " + client->ip + " failed to execute NICK (Erroneus nickname).", RED_CMD);
		else
			Response::createReply(ERR_ERRONEUSNICKNAME).From(server).To(*client).Command(parameter).Trailer("Erroneus nickname").Send("[-]Client: " + client->nick + " failed to execute NICK (Erroneus nickname).", RED_CMD);
	}
	else //check if the nickname is already in use
	{
		check = server.getClientByNick(parameter);
		if (check) //nickname already in use
			Response::createReply(ERR_NICKNAMEINUSE).From(server).To(*client).Command(parameter).Trailer("Nickname is already in use").Send("[-]Client: " + client->nick + " failed to execute NICK (Nickname is already in use).", RED_CMD);
		else //nickname is available
		{
            if (client->channels.size())
			    Response::createMessage().From(*client).Command("NICK " + parameter).Broadcast(client->channels, false);
            Response::createMessage().From(*client).To(*client).Command("NICK " + parameter).Send("[!]Client: " + client->nick + " changed nickname to: " + parameter, YELLOW_CMD);
			client->nick = parameter;
		}
	}
}

void Commands::execUser( const std::string & parameter, Client * client, Server & server )
{
	std::string::size_type space, colon;

	space = parameter.find(" ");
	colon = parameter.find(":");
	if (parameter.empty()) //not enough parameters
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("USER").Trailer("Not enough parameters").Send("[-]Client: " + client->nick + " failed to execute USER (Not enough parameters).", RED_CMD);
	else if (client->status >= CONNECTED) //already connected
		Response::createReply(ERR_ALREADYREGISTERED).From(server).To(*client).Trailer("You may not reregister").Send("[-]Client: " + client->nick + " already connected (You may not reregister).", RED_CMD);
	else //set the user and realname
	{
		if (space == std::string::npos)
			 client->user = client->nick;
		else
			client->user = parameter.substr(0, space);
		if (colon == std::string::npos)
			client->realName = client->nick;
		else
			client->realName = parameter.substr(colon + 1, parameter.size());
		if (DEBUG)
			addFileLog("[/]Client: " + client->nick + " from: " + client->ip + " User: " + client->user + " Realname: " + client->realName, BLUE_CMD);
	}
}

void	Commands::execWhois( const std::string & parameter, Client * client, Server & server )
{
	std::string::size_type space;
	Client *	clientWho = NULL;

	space = parameter.find(" ");
	if (space != std::string::npos)
		clientWho = server.getClientByNick(parameter.substr(space + 1));
	if (clientWho)
	{
		Response::createMessage().From(server).To(*client).Command("311 " + clientWho->nick + " " + clientWho->nick + " " + clientWho->user + " " + server.serverHost + "/" + clientWho->nick + " *").Trailer(clientWho->realName).Send();
		//Response::createMessage().From(server).To(*client).Command(RPL_WHOISUSER + " " + clientWho->nick + " " + clientWho->nick + " " + clientWho->user + " " + server.serverHost + "/" + clientWho->nick + " *").Trailer(clientWho->realName).Send();
		Response::createMessage().From(server).To(*client).Command("318 " + clientWho->nick + " " + clientWho->nick).Trailer("End of /WHOIS list.").Send();
		//Response::createMessage().From(server).To(*client).Command(RPL_ENDOFWHOIS + " " + clientWho->nick + " " + clientWho->nick).Trailer("End of /WHOIS list.").Send();
	}
	else
		std::cout << "Failed who to " << parameter << std::endl;
};


void Commands::execQuit( const std::string & parameter, Client * client, Server & server )
{
	std::map< Channel *, std::string >::iterator	channel;

	Response::createMessage().From(server).To(*client).Command("ERROR").Trailer("Client close connection.").Send("[!]Client: " + client->nick + " from: " + client->ip + " close connection.", YELLOW_CMD);
	if (client->channels.size())
		Response::createMessage().From(*client).Command("QUIT").Trailer(parameter).Broadcast(client->channels, false);
	for (channel = client->channels.begin(); channel != client->channels.end(); channel++)
		channel->first->delClient(client, server);
	client->channels.clear();
	client->status = DISCONECT;
}

