#include "Commands.hpp"
#include "Server.hpp"

Commands::Commands( void )
{
    commandMap["CAP"] = CAP;
    commandMap["PASS"] = PASS;
    commandMap["NICK"] = NICK;
    commandMap["USER"] = USER;
    commandMap["JOIN"] = JOIN;

    commands[CAP].exec = &execCap;
    commands[PASS].exec = &execPass;
    commands[NICK].exec = &execNick;
    commands[USER].exec = &execUser;
    commands[JOIN].exec = &execJoin;
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
    if (!client.authpass || client.nick == "" || client.user == "" || client.realName == "")
        return false;
    client.status = CONNECTED;
    return true;
}

bool    Commands::processInput( const std::string & input, Client & client, Server & server )
{
    std::string::size_type endLine, space, startLine;
    endLine = input.find("\n");
    std::string line = "";
    startLine = 0;
    while (endLine != std::string::npos)// || readBuffer[endLine - 1] != '\r')
    {
        line = input.substr(startLine, endLine - startLine - 1);
        //std::cout << "Line: " << line << std::endl;
        space = line.find(" ");
        if (space != std::string::npos)
        {
            if (!execCmd(line.substr(0, space), line.substr(space + 1, line.size()), client, server))
                return false;

        }
        startLine = endLine + 1;
        endLine = input.find('\n', startLine);
    }
    if (client.status == AUTH)
    {
        if (checkLogin(client))
        {
            Response response;
            //send welcome message.
            std::cout << "Client: " << client.nick << " connected.\n";
            std::string msg = "";

            
            response.createReply(RPL_WELCOME).From(server).To(client).Content("Welcome to irc server.").Send();



            //Response::createReply(RPL_WELCOME).To(client).Content("Welcome to irc server. " + client.nick).Send();
            // msg = ":" + server.serverName + "." + server.serverHost + " 001 " + client.nick + " :Welcome to irc server.\r\n";
            // std::cout << "RESPONSE: " << msg;
            // send(client.fd, msg.c_str(), msg.size(), 0);
            // we have to check in cap if host is *

            if (client.host == "")
            {
                client.host = client.user + "@" + server.serverHost;
                //not sure to need this..
                response.createReply(ERR_HOST).From(server).To(client).Content("This is now your displayed host " + server.serverHost).Send();
            }
            //msg = ":" + server.serverName + "." + server.serverHost + " 396 " + client.nick + " " + server.serverHost + " :is now your displayed host\r\n";
            //std::cout << "RESPONSE: " << msg;
            //send(client.fd, msg.c_str(), msg.size(), 0);
            //add to the client map
            server.clients[client.nick] = client;
        }
        if (input.find("CAP") != std::string::npos && client.status != CONNECTED)
        {
            //send failed to connect, but every single command send his message.
            std::cout << "Client: " << client.fd << " failed to connect.\n";
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
    else
        return commands[cmd].exec(argument, client, server);
    return true;
}

//All commands of theserver.

bool        Commands::execCap( const std::string & argument, Client & client, Server & server )
{
    (void)argument;
    (void)client;
    (void)server;
    std::cout << "Processing CAP\n";
    return true;
}

bool        Commands::execPass( const std::string & argument, Client & client, Server & server )
{
    //Check that pass it's ok.
    std::cout << "Checking pass... " << argument << std::endl;
    if (argument == server.getPassword())
    {
        std::cout << "Pass ok.\n";
        client.status = AUTH;
        //this may be delete
        client.authpass = true;
    }
    else
        std::cout << "Wrong pass: " << server.getPassword() << std::endl;
    return client.authpass;
}

bool Commands::execNick( const std::string & argument, Client & client, Server & server )
{
    (void)server;
    //Check that nick is not used.
    // else return user change name error.
    if (server.clients.find(argument) == server.clients.end())
    {
        server.clients.erase(argument);
        std::cout << "Client: " << client.fd << " changed Nick from: " << client.nick;
        client.nick = argument;
        std::cout << " to: " << client.nick << std::endl;
        server.clients[argument] = client;
    }
    else
    {
        //return error response nick in use. ERR_NICKNAMEINUSE (433)
        Response    response;

        response.createReply(ERR_NICKNAMEINUSE).From(server).To(client).Content("Nickname " + argument + " is already in use").Send();
        // std::string msg = "";
        // msg = msg = ":" + server.serverName + "." + server.serverHost + " 433 " + client.nick + " " + argument + " :Nickname is already in use\r\n";
        // send(client.fd, msg.c_str(), msg.size(), 0);
        // std::cout << "RESPONSE: " << msg;
        // std::cout << "Nick " << argument << " is in use.\n";
        return false;
    }
    return true;
}

bool Commands::execUser( const std::string & argument, Client & client, Server & server )
{
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
        std::cout << "User: " << client.user << " Realname: " << client.realName << std::endl;
    }
    return true;
}

bool        Commands::execJoin( const std::string & argument, Client & client, Server & server )
{
    std::string msg = "";

    //only to test join, we don't check anything, only send like we created the channel
    msg = ":" + client.nick + "!" + client.host + " JOIN " + argument + " * :" + client.realName + "\r\n";
    std::cout << "RESPONSE: " << msg;
    send(client.fd, msg.c_str(), msg.size(), 0);
    // here send the name of clients in the channel. In this test, create one new and cleint is the operator "@" is the mode. +otroNick is a standar client to test.
    msg = ":" + server.serverName + "." + server.serverHost + " 353 " + client.nick + " = " + argument + " :@" + client.nick + "!" + client.host + " +otroNick\r\n";
    std::cout << "RESPONSE: " << msg;
    send(client.fd, msg.c_str(), msg.size(), 0);
    // last send the end fo list messagge.
    msg = ":" + server.serverName + "." + server.serverHost + " 366 " + client.nick + " " + argument + " :End of name list.\r\n";
    std::cout << "RESPONSE: " << msg;
    send(client.fd, msg.c_str(), msg.size(), 0);
    // if can't join return false
    return true;
}