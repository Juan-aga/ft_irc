#include "Commands.hpp"
#include "Server.hpp"

void        Commands::execCap( const std::string & parameter, Client & client, Server & server )
{
	//we don't handle 
	(void)parameter;
	(void)client;
	(void)server;
}

//if PASS fails, we have to send the response and close conection.
void        Commands::execPass( const std::string & parameter, Client & client, Server & server )
{
	if (parameter.empty())
	{
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(client).Command("PASS").Trailer("Not enough parameters").Send();
		addFileLog("[-]Client from ip: " + client.ip + " failed to connect (Not enough parameters).", RED_CMD);
	}
	else if (client.status >= CONNECTED)
	{
		Response::createReply(ERR_ALREADYREGISTERED).From(server).To(client).Trailer("You may not reregister").Send();
		addFileLog("[-]Client from ip: " + client.ip + " failed to connect (You may not reregister).", RED_CMD);
	}
	else if (parameter == server.getPassword())
		client.status = AUTH;
	else
	{
		Response::createReply(ERR_PASSWDMISMATCH).From(server).To(client).Trailer("Password incorrect").Send();
		addFileLog("[-]Client from ip: " + client.ip + " failed to connect (incorrect password).", RED_CMD);
		client.status = DISCONECT;
	}
}

void Commands::execNick( const std::string & parameter, Client & client, Server & server )
{
	Client * check;
	
	if (parameter.empty())
		Response::createReply(ERR_NONICKNAMEGIVEN).From(server).To(client).Trailer("No nickname given").Send();
	else if (!parseNick(parameter))
		Response::createReply(ERR_ERRONEUSNICKNAME).From(server).To(client).Command(parameter).Trailer("Erroneus nickname").Send();
	else
	{
		check = server.getClientByNick(parameter);
		if (check)
			Response::createReply(ERR_NICKNAMEINUSE).From(server).To(client).Command(parameter).Trailer("Nickname is already in use").Send();
		else
		{
			// we need to propagate the change to the channels.
			if (DEBUG)	
				std::cout << "Client: " << client.fd << " changed Nick from: " << client.nick << " to: " << parameter << std::endl;
			client.nick = parameter;
		}
	}
}

void Commands::execUser( const std::string & parameter, Client & client, Server & server )
{
	std::string::size_type space, colon;
	
	space = parameter.find(" ");
	colon = parameter.find(":");
	if (parameter.empty())
		  Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(client).Command("USER").Trailer("Not enough parameters").Send();
	else if (client.status >= CONNECTED)
		Response::createReply(ERR_ALREADYREGISTERED).From(server).To(client).Trailer("You may not reregister").Send();
	else
	{
		if (space == std::string::npos)
			 client.user = client.nick;
		   else
			client.user = parameter.substr(0, space);
		if (colon == std::string::npos)
			client.realName = client.nick;
		else
			client.realName = parameter.substr(colon + 1, parameter.size());
		if (DEBUG)
			std::cout << "User: " << client.user << " Realname: " << client.realName << std::endl;
	}
}