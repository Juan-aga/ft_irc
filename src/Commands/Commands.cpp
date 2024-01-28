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

bool    Commands::checkLogin( Client & client )
{
    if (client.status != AUTH || client.nick == "" || client.user == "" || client.realName == "")
        return false;
    if (DEBUG)
    	std::cout << "Client " << client.nick << " conected in FD: " << client.fd << std::endl;
    client.status = CONNECTED;
    return true;
}

bool    Commands::processInput( const std::string & input, Client & client, Server & server )
{
    std::string::size_type endLine, space, startLine;
    endLine = input.find("\n");
    std::string line = "";
    startLine = 0;
    
    while (endLine != std::string::npos)
    {
        line = input.substr(startLine, endLine - startLine - 1);
        space = line.find(" ");
        if (space != std::string::npos && client.status != DISCONECT)
            execCmd(line.substr(0, space), line.substr(space + 1, line.size()), client, server);
        startLine = endLine + 1;
        endLine = input.find('\n', startLine);
    }
    if (client.status == AUTH)
    {
        if (checkLogin(client))
        {
            //send welcome message.
            Response::createReply(RPL_WELCOME).From(server).To(client).Trailer("Welcome to irc server.").Send();
            //think we don't need the host...
            if (client.host == "")
            {
                client.host = client.user + "@" + server.serverHost;
                //not sure to need this..
                Response::createReply(ERR_HOST).From(server).To(client).Command(server.serverHost).Trailer("This is now your displayed host").Send();
            }
        }
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
    {
        //command not founnd.
        std::cout << "Command " << command << " not found.\nArguments: " << argument << std::endl;
        // discoment when all commands implemented
        //return false;
    }
    // we have to implement the check if client is auth (except for pass)
    // if it's not auth, we have to send a response and don't do anything.
    else if (client.status == DISCONECT)
    {
    	std::cout << "Client was desconnected from the server.\n";
     	return false;
    }
    else if ((client.status == UNKNOWN && cmd != PASS) || (client.status == AUTH && cmd >= JOIN))
    {
    	//not auth to do the command
     	//not sure if we have to send a response to the client.
     	std::cout << "Not authorized to execute " << command << std::endl;
      	return false;
    }
    else
        return commands[cmd].exec(argument, client, server);
    return true;
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
	std::map<int, Client *>::iterator	it;
    
	it = server.clients.begin();
	for (; it != server.clients.end(); it++)
	{
		if (it->second->nick == argument)
			break;
	}
	if (it != server.clients.end())
	{
  		//return error response nick in use. ERR_NICKNAMEINUSE (433)
        Response::createReply(ERR_NICKNAMEINUSE).From(server).To(client).Command(argument).Trailer("Nickname is already in use").Send();
        return false;
	}
	else
	{
		if (DEBUG)	
        	std::cout << "Client: " << client.fd << " changed Nick from: " << client.nick << " to: " << argument << std::endl;
         client.nick = argument;
         return true;
	}
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
    // wen we define channel class, we have to check if it exists, and the permissions on the channel.
    std::map< int, Channel * >::iterator    it;
    int index;

    it = server.channels.begin();
    for (; it != server.channels.end(); it++)
    {
        if (it->second->name == argument)
            break;
    }
    //create a new channel;
    if (it == server.channels.end())
    {
        index = Channel::totalCount;
        server.channels[index] = new Channel(argument, & client);
    }
    else
    {
        //it's on channel
        if (it->second->isClient(client.nick, server))
        {
            std::cout << "Client " << client.nick << " is already a member of " << it->second->name << std::endl;
            return false;
        }
        //we have to check permissions.
        else
            it->second->addClient(&client);
        index = it->first;
    }


    //only to test join, we don't check anything, only send like we created the channel
    // not sure about last *, we have to check if it's the mode.
    // wen we implemented broadcast, this response is for all the clients in the channel.
    // Response::createMessage().From(client).To(client).Command("JOIN " + server.channels[index]->name + " *").Send();

    /// test until broadcast.

    std::map<Client *, std::string>::iterator gclients;
    std::string msg = "";

    gclients = server.channels[index]->clients.begin();
    for (; gclients != server.channels[index]->clients.end(); gclients++)
    {
        Response::createMessage().From(client).To(*gclients->first).Command("JOIN " + server.channels[index]->name + " " + server.channels[index]->clients[&client]).Send();
        msg += gclients->second + gclients->first->nick + " ";
    }


    // // here send a list of name clients in the channel. In this test, create one new and client is the operator "@" is the mode. +otroNick is a standar client to test.
    // Response::createReply(RPL_NAMREPLY).From(server).To(client).Command("= " + argument).Trailer("@" + client.nick + " +otroNick").Send();

    Response::createReply(RPL_NAMREPLY).From(server).To(client).Command("= " + argument).Trailer(msg).Send();

    // last send the end of list messagge.
    Response::createReply(RPL_ENDOFNAMES).From(server).To(client).Command(argument).Trailer("End of name list.").Send();

    // test only send another client has joined 
    //Response::createMessage().Trailer("meloinvenTo JOIN " + argument).Send();

    return true;
}

bool    Commands::execPrivmsg( const std::string & argument, Client & client, Server & server )
{
    std::string to, msg;
    std::string::size_type space, colon;

    space = argument.find(" ");
    colon = argument.find(":");
    if (space == std::string::npos || colon == std::string::npos)
    {
        std::cout << "Error parsing PRIVMSG.\n";
        return false;
    }
    else
    {
        to = argument.substr(0, space);
        msg = argument.substr(colon + 1, argument.size());
        //check if channel exist. can create a method in the server class;
        std::map< int, Channel * >::iterator    it;

        it = server.channels.begin();
        for (; it != server.channels.end(); it++)
        {
            if (it->second->name == to)
                break;
        }
        if (it != server.channels.end())
        {
            if (it->second->isClient(client.nick, server))
            {
                //send broadcast to all the channel
                    std::map<Client *, std::string>::iterator gclients;

                    //works if we check that channels must start with #, we have to implement the checkers for valid characters for users and channels.
                    for (gclients = it->second->clients.begin(); gclients != it->second->clients.end(); gclients++)
                    {
                        if (gclients->first->nick != client.nick)
                            Response::createMessage().From(client).To(*gclients->first).Command("PRIVMSG " + to + " " + msg).Send();
                    }
            }
            else
            {
                //the client is not on the channel
                std::cout << client.nick << " is not in " << it->second->name << " can't send messages.\n";
                return false;
            }
        }
        //check if is a client
        else
        {
            std::map<int, Client *>::iterator	it;
    
            it = server.clients.begin();
            for (; it != server.clients.end(); it++)
            {
                if (it->second->nick == to)
                    break;
            }
            if (it != server.clients.end())
            {
                //send to this client:
                Response::createMessage().From(client).To(*it->second).Command("PRIVMSG " + to).Trailer(msg).Send();
            }
            else
            {
                //the client don't exist
                std::cout << to << " client don't exist.\n";
                return false;
            }
        }
    }
    return true;
}


bool Commands::execKill( const std::string & argument, Client & client, Server & server )
{
    (void)argument;

    server.stopServer();

    std::cout << "Stopping server from: " << client.nick << std::endl;
    return true;
}