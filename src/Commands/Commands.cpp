#include "Commands.hpp"
#include "Server.hpp"

Commands::Commands( void )
{
	commandMap["CAP"] = CAP;
	commandMap["PASS"] = PASS;
	commandMap["NICK"] = NICK;
	commandMap["USER"] = USER;
	commandMap["JOIN"] = JOIN;
	commandMap["PRIVMSG"] = PRIVMSG;
	commandMap["KILLSERVER"] = KILLSERVER;

	commands[CAP].exec = &execCap;
	commands[PASS].exec = &execPass;
	commands[NICK].exec = &execNick;
	commands[USER].exec = &execUser;
	commands[JOIN].exec = &execJoin;
	commands[PRIVMSG].exec = &execPrivmsg;
	commands[KILLSERVER].exec = &execKill;
}

Commands::~Commands( void )
{

}

Commands::_CMD  Commands::strToCmd( const std::string & cmd )
{
	std::map<std::string, _CMD>::const_iterator it = commandMap.find(cmd);

	if (it != commandMap.end())
		return it->second;
	else
		return MAX_CMD;
}

bool    Commands::checkLogin( Client & client, Server const & server )
{
	if (client.status != AUTH || client.nick == "" || client.user == "" || client.realName == "")
		return false;
	if (DEBUG)
		std::cout << "Client " << client.nick << " conected in FD: " << client.fd << std::endl;
	client.status = CONNECTED;
	Response::createReply(RPL_WELCOME).From(server).To(client).Trailer("Welcome to irc server.").Send();
	addFileLog("[+]Client: " + client.nick + " from: " + client.ip + " Conected.", GREEN_CMD);
	//think we don't need the host...
	if (client.host == "")
	{
		client.host = client.user + "@" + server.serverHost;
		//not sure to need this..
		Response::createReply(ERR_HOST).From(server).To(client).Command(server.serverHost).Trailer("This is now your displayed host").Send();
	}
	return true;
}

void    Commands::processInput( const std::string & input, Client & client, Server & server )
{
	std::string::size_type endLine, space, startLine;
	endLine = input.find("\n");
	std::string line = "";
	startLine = 0;
	
	while (endLine != std::string::npos && client.status != DISCONECT)
	{
		line = input.substr(startLine, endLine - startLine - 1);
		space = line.find(" ");
		if (space != std::string::npos )
			execCmd(line.substr(0, space), line.substr(space + 1, line.size()), client, server);
		else
		// need to handle parameters in every command, someone don't need parameters.
			execCmd(line, "", client, server);
		startLine = endLine + 1;
		endLine = input.find('\n', startLine);
	}
	if (client.status == AUTH)
	{
		checkLogin(client, server);
		if (input.find("CAP") != std::string::npos && client.status != CONNECTED)
		{
			//send failed to connect, but every single command send his message.
			if (DEBUG)
				std::cout << "Client: " << client.nick << " failed to connect from FD: " << client.fd << std::endl;
		}
	}
}

void    Commands::execCmd( const std::string & command, const std::string & parameter, Client & client, Server & server )
{
	_CMD    cmd;

	cmd = strToCmd(command);
	if (cmd == MAX_CMD)
		addFileLog("[-]Command " + command + " not found. Arguments: " + parameter, RED_CMD);
	else if (client.status == DISCONECT)
		addFileLog("[-]Client from ip: " + client.ip + " failed to connect (Disconnected).", RED_CMD);
	else if ((client.status == UNKNOWN && cmd > CAP) || (client.status == AUTH && cmd >= JOIN))
	{
		//not auth to do the command
		 //not sure if we have to send a response to the client.
		 std::cout << "Not authorized to execute " << command << std::endl;
	}
	else
	{
		addFileLog("[+]Command: " + command + " from: " + client.nick + " Arguments {" + parameter + "} Executed.", GREEN_CMD);
		commands[cmd].exec(parameter, client, server);
	}
}

//All commands of the server.

void Commands::execKill( const std::string & parameter, Client & client, Server & server )
{
	(void)parameter;

	server.stopServer();

	std::cout << "Stopping server from: " << client.nick << std::endl;
}

bool Commands::parseNick( const std::string & parameter)
{
	// check leading chaaracters first
	if (parameter[0] == '#'  || parameter[0] == ':')
		return false;
	// check if there is an ascii space <' '>	
	for (int i = 0; i < int(parameter.size()); i++)
	{
		if (parameter[i] == ' ')
			return false;
	}
	
	return true;
}