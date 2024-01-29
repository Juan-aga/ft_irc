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
        std::cout << "Command " << command << " not found.\nArguments: " << parameter << std::endl;
    else if (client.status == DISCONECT)
    	std::cout << "Client was desconnected from the server.\n";
    else if ((client.status == UNKNOWN && cmd > CAP) || (client.status == AUTH && cmd >= JOIN))
    {
    	//not auth to do the command
     	//not sure if we have to send a response to the client.
     	std::cout << "Not authorized to execute " << command << std::endl;
    }
    else
        commands[cmd].exec(parameter, client, server);
}

//All commands of the server.

void        Commands::execCap( const std::string & parameter, Client & client, Server & server )
{
    (void)parameter;
    (void)client;
    (void)server;
    //we don't handle 
    if (DEBUG)
    	std::cout << "Processing CAP\n";
}

//if PASS fails, we have to send the response and close conection.
void        Commands::execPass( const std::string & parameter, Client & client, Server & server )
{
	if (parameter.empty())
		Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(client).Command("PASS").Trailer("Not enough parameters").Send();
	else if (client.status >= CONNECTED)
  		Response::createReply(ERR_ALREADYREGISTERED).From(server).To(client).Trailer("You may not reregister").Send();
	else if (parameter == server.getPassword())
    {
    	if (DEBUG)
        	std::cout << "Pass is ok.\n";
        client.status = AUTH;
    }
    else
    {
    	Response::createReply(ERR_PASSWDMISMATCH).From(server).To(client).Trailer("Password incorrect").Send();
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
    // we have to implement to take the host. If it's * let it empty, it's catched later.
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

void        Commands::execJoin( const std::string & parameter, Client & client, Server & server )
{
    Channel *   channel;
    
    channel = server.getChannelByName(parameter);
    if (parameter[0] != '#')
    {
    	std::cout << parameter << "is not a valid channel name" << std::endl;
    	//if channel's name is not valid send a respionse	
    }
    else if (channel)
    {
        if (channel->isClient(client.nick))
        {
            std::cout << "Client " << client.nick << " is already a member of " << channel->name << std::endl;
        }
        //we have to check permmisions
        else
            channel->addClient(&client, server);
    }
    else
        server.channels.push_back(new Channel(parameter, & client, server));
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