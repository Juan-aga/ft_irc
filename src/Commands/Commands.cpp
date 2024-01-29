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

bool    Commands::processInput( const std::string & input, Client & client, Server & server )
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
            return false;
        }
    }
    return true;
}

bool        Commands::execCmd( const std::string & command, const std::string & argument, Client & client, Server & server )
{
    _CMD    cmd;

    cmd = strToCmd(command);
    if (cmd == MAX_CMD)
        std::cout << "Command " << command << " not found.\nArguments: " << argument << std::endl;
    else if (client.status == DISCONECT)
    	std::cout << "Client was desconnected from the server.\n";
    else if ((client.status == UNKNOWN && cmd > CAP) || (client.status == AUTH && cmd >= JOIN))
    {
    	//not auth to do the command
     	//not sure if we have to send a response to the client.
     	std::cout << "Not authorized to execute " << command << std::endl;
    }
    else
        return commands[cmd].exec(argument, client, server);
    return false;
}

//All commands of the server.

bool        Commands::execCap( const std::string & argument, Client & client, Server & server )
{
    (void)argument;
    (void)client;
    (void)server;
    //we don't handle 
    if (DEBUG)
    	std::cout << "Processing CAP\n";
    return true;
}

//if PASS fails, we have to send the response and close conection.
bool        Commands::execPass( const std::string & argument, Client & client, Server & server )
{
    if (argument == server.getPassword())
    {
    	if (DEBUG)
        	std::cout << "Pass is ok.\n";
        client.status = AUTH;
        return true;
    }
    else
    {
        // we have to implement the response to the client.
        if (DEBUG)
        	std::cout << "Wrong pass: " << server.getPassword() << std::endl;
        // if pass fail send ERR_PASSWDMISMATCH (464) and close conection.
        client.status = DISCONECT;
        return false;
    }
}

bool Commands::execNick( const std::string & argument, Client & client, Server & server )
{
    Client * check;

    check = server.getClientByNick(argument);
    if (check)
        Response::createReply(ERR_NICKNAMEINUSE).From(server).To(client).Command(argument).Trailer("Nickname is already in use").Send();
    else
	{
        // we need to propagate the change to the channels.
		if (DEBUG)	
        	std::cout << "Client: " << client.fd << " changed Nick from: " << client.nick << " to: " << argument << std::endl;
         client.nick = argument;
         return true;
	}
    return false;
}

bool Commands::execUser( const std::string & argument, Client & client, Server & server )
{
    // we have to implement to take the host. If it's * let it empty, it's catched later.
    (void)server;
    std::string::size_type space, colon;
    
    space = argument.find(" ");
    colon = argument.find(":");
    if (space == std::string::npos || colon == std::string::npos)
    {
        //error on user get user o realname.
        //check if we haver to send a reply
        std::cout << "Error parsing user or realname.\n";
        return false;
    }
    else
    {
        client.user = argument.substr(0, space);
        client.realName = argument.substr(colon + 1, argument.size());
        //changed user and realname.
        // host???
        if (DEBUG)
        	std::cout << "User: " << client.user << " Realname: " << client.realName << std::endl;
    }
    return true;
}

bool        Commands::execJoin( const std::string & argument, Client & client, Server & server )
{
    Channel *   channel;

    channel = server.getChannelByName(argument);
    if (channel)
    {
        if (channel->isClient(client.nick))
        {
            std::cout << "Client " << client.nick << " is already a member of " << channel->name << std::endl;
            return false;
        }
        //we have to check permmisions
        else
            channel->addClient(&client, server);
    }
    else
        server.channels[Channel::totalCount] = new Channel(argument, & client, server);
    return true;
}

bool    Commands::execPrivmsg( const std::string & argument, Client & client, Server & server )
{
    std::string to, msg;
    std::string::size_type space, colon;

    //maybe we have to do a function for this if there are another commands that need it.
    space = argument.find(" ");
    colon = argument.find(":");
    if (space == std::string::npos || colon == std::string::npos)
    {
        std::cout << "Error parsing PRIVMSG.\n";
        return false;
    }
    else
    {
        Channel *   channel;
        
        to = argument.substr(0, space);
        msg = argument.substr(colon + 1, argument.size());
        channel = server.getChannelByName(to);

        if (channel)
        {
            if (channel->isClient(client.nick))
                Response::createMessage().From(client).Command("PRIVMSG " + to + " " + msg).Broadcast(channel->clients, false);
            else
            {
                //the client is not on the channel
                std::cout << client.nick << " is not in " << channel->name << " can't send messages.\n";
                return false;
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
                return false;
            }
        }
        return true;
    }
}

bool Commands::execKill( const std::string & argument, Client & client, Server & server )
{
    (void)argument;

    server.stopServer();

    std::cout << "Stopping server from: " << client.nick << std::endl;
    return true;
}