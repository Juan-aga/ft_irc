#include "Commands.hpp"
#include "Server.hpp"

void        Commands::execCap( const std::string & parameter, Client * client, Server & server )
{
	//we don't handle 
	(void)parameter;
	(void)client;
	(void)server;
	addFileLog("[-]Client from ip: " + client->ip + " trying CAP: " + parameter + ". Not supported.", RED_CMD);
}

void        Commands::execPass( const std::string & parameter, Client * client, Server & server )
{
	if (parameter.empty())
	{
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("PASS").Trailer("Not enough parameters").Send();
		addFileLog("[-]Client from ip: " + client->ip + " failed to connect (Not enough parameters).", RED_CMD);
	}
	else if (client->status >= CONNECTED)
	{
		Response::createReply(ERR_ALREADYREGISTERED).From(server).To(*client).Trailer("You may not reregister").Send();
		addFileLog("[-]Client: " + client->nick + " already connected (You may not reregister).", RED_CMD);
	}
	else if (parameter == server.getPassword())
		client->status = AUTH;
	else
	{
		Response::createReply(ERR_PASSWDMISMATCH).From(server).To(*client).Trailer("Password incorrect").Send();
		addFileLog("[-]Client from ip: " + client->ip + " failed to connect (incorrect password).", RED_CMD);
		client->status = DISCONECT;
	}
}

void Commands::execNick( const std::string & parameter, Client * client, Server & server )
{
	Client * check;
	
	if (parameter.empty())
	{
		Response::createReply(ERR_NONICKNAMEGIVEN).From(server).To(*client).Trailer("No nickname given").Send();
		if (client->nick == "")
			addFileLog("[-]Client from ip: " + client->ip + " failed to execute NICK (No nickname given).", RED_CMD);
		else
			addFileLog("[-]Client: " + client->nick + " failed to execute NICK (No nickname given).", RED_CMD);
	}
	else if (!parseNick(parameter))
	{
		Response::createReply(ERR_ERRONEUSNICKNAME).From(server).To(*client).Command(parameter).Trailer("Erroneus nickname").Send();
		if (client->nick == "")
			addFileLog("[-]Client from ip: " + client->ip + " failed to execute NICK (Erroneus nickname).", RED_CMD);
		else
			addFileLog("[-]Client: " + client->nick + " failed to execute NICK (Erroneus nickname).", RED_CMD);
	}
	else
	{
		check = server.getClientByNick(parameter);
		if (check)
			Response::createReply(ERR_NICKNAMEINUSE).From(server).To(*client).Command(parameter).Trailer("Nickname is already in use").Send();
		else
		{
            if (client->channels.size())
			    Response::createMessage().From(*client).Command("NICK " + parameter).Broadcast(client->channels, false);
            Response::createMessage().From(*client).To(*client).Command("NICK " + parameter).Send();
			client->nick = parameter;
		}
	}
}

void Commands::execUser( const std::string & parameter, Client * client, Server & server )
{
	std::string::size_type space, colon;
	
	space = parameter.find(" ");
	colon = parameter.find(":");
	if (parameter.empty())
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("USER").Trailer("Not enough parameters").Send();
	else if (client->status >= CONNECTED)
		Response::createReply(ERR_ALREADYREGISTERED).From(server).To(*client).Trailer("You may not reregister").Send();
	else
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
			std::cout << "User: " << client->user << " Realname: " << client->realName << std::endl;
	}
}

void Commands::execQuit( const std::string & parameter, Client * client, Server & server )
{
	std::map< Channel *, std::string >::iterator	channel;

	Response::createMessage().From(server).To(*client).Command("ERROR").Trailer("Client close connection.").Send();
	if (client->channels.size())
		Response::createMessage().From(*client).Command("QUIT").Trailer(parameter).Broadcast(client->channels, false);
	for (channel = client->channels.begin(); channel != client->channels.end(); channel++)
		channel->first->delClient(client, server);
	client->channels.clear();
	client->status = DISCONECT;
}