#include "Commands.hpp"
#include "Server.hpp"

Commands::Commands( void )
{
	commandMap["CAP"] = CAP;
	commandMap["PASS"] = PASS;
	commandMap["NICK"] = NICK;
	commandMap["USER"] = USER;
	commandMap["QUIT"] = QUIT;
	commandMap["JOIN"] = JOIN;
	commandMap["PRIVMSG"] = PRIVMSG;
	commandMap["KILLSERVER"] = KILLSERVER;
	commandMap["TOPIC"] = TOPIC;
	commandMap["PART"] = PART;
	commandMap["KICK"] = KICK;
	commandMap["MODE"] = MODE;
	commandMap["INVITE"] = INVITE;
	commandMap["WHO"] = WHO;

	commands[CAP].exec = &execCap;
	commands[PASS].exec = &execPass;
	commands[NICK].exec = &execNick;
	commands[USER].exec = &execUser;
	commands[QUIT].exec = &execQuit;
	commands[JOIN].exec = &execJoin;
	commands[PRIVMSG].exec = &execPrivmsg;
	commands[KILLSERVER].exec = &execKill;
	commands[TOPIC].exec = &execTopic;
	commands[PART].exec = &execPart;
	commands[KICK].exec = &execKick;
	commands[MODE].exec = &execMode;
	commands[INVITE].exec = &execInvite;
	commands[WHO].exec = &execWho;
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

bool    Commands::checkLogin( Client * client, Server const & server )
{
	if (client->status != AUTH || client->nick == "" || client->user == "" || client->realName == "")
		return false;
	client->status = CONNECTED;
	Response::createReply(RPL_WELCOME).From(server).To(*client).Trailer("Welcome to irc server.").Send();
	addFileLog("[+]Client: " + client->nick + " from: " + client->ip + " Conected.", GREEN_CMD);
	return true;
}

void    Commands::processInput( const std::string & input, Client * client, Server & server )
{
	std::string::size_type endLine, space, startLine;
	endLine = input.find("\n");
	std::string line = "";
	startLine = 0;
	
	while (endLine != std::string::npos && client->status != DISCONECT)
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
	if (client->status == AUTH)
	{
		checkLogin(client, server);
		if (input.find("CAP") != std::string::npos && client->status != CONNECTED)
		{
			//send failed to connect, but every single command send his message.
			if (DEBUG)
				std::cout << "Client: " << client->nick << " failed to connect from FD: " << client->fd << std::endl;
		}
		else
			server.channels[0]->addClient(client, server);
	}
}

void    Commands::execCmd( const std::string & command, const std::string & parameter, Client * client, Server & server )
{
	_CMD    cmd;

	cmd = strToCmd(command);
	//if the command is not found, we have to send a response to the client.
	if (cmd == MAX_CMD)
	{
		Response::createReply(ERR_UNKNOWNCOMMAND).From(server).To(*client).Command(command).Trailer("Unknown command").Send();
		addFileLog("[-]Command: " + command + " not found. Arguments: " + parameter, RED_CMD);
	}
	else if (client->status == DISCONECT)
	//this is not "failed to connect", it was disconnected by the server.
		addFileLog("[!]Client from ip: " + client->ip + " disconnected by the server", YELLOW_CMD);
	else if ((client->status == UNKNOWN && cmd > CAP) || (client->status == AUTH && cmd >= JOIN))
	{
		//not auth to do the command
		 //not sure if we have to send a response to the client.
		 std::cout << "Not authorized to execute " << command << std::endl;
	}
	else
	{
		if (command != "PRIVMSG" && command != "PASS" && command != "CAP")
		{
			if(client->nick != "")
				addFileLog("[!]Client: " + client->nick + " from: " + client->ip + " Command: " + command + " Arguments: " + parameter, YELLOW_CMD);
			else
				addFileLog("[!]Client: " + client->ip + " Command: " + command + " Arguments: " + parameter, YELLOW_CMD);
		}
		commands[cmd].exec(parameter, client, server);
	}
}

//All commands of the server.

void Commands::execKill( const std::string & parameter, Client * client, Server & server )
{
	(void)parameter;

	server.stopServer();

	std::cout << "Stopping server from: " << client->nick << std::endl;
}

bool Commands::parseNick( const std::string & parameter)
{
	// check leading chaaracters first
	if (parameter[0] == '#'  || parameter[0] == ':')
		return false;
	// check if there is an ascii space <' '>	
	for (int i = 0; i < int(parameter.size()); i++)
	{
		if (parameter[i] == ' ' || parameter[i] == ',')
			return false;
	}
	
	return true;
}