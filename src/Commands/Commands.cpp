#include "Commands.hpp"
#include "Server.hpp"

Commands::Commands( void )
{
    commandMap["CAP"] = CAP;
    commandMap["PASS"] = PASS;
    commandMap["NICK"] = NICK;
    commandMap["USER"] = USER;

    commands[CAP].exec = &execCap;
    commands[PASS].exec = &execPass;
    commands[NICK].exec = &execNick;
    commands[USER].exec = &execUser;
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
    {
        client.nick = "";
        client.user = "";
        client.realName = "";
        return false;
    }
    return true;
}

void    Commands::processInput( const std::string & input, Client & client, Server & server )
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
            execCmd(line.substr(0, space), line.substr(space + 1, line.size()), client, server);
        startLine = endLine + 1;
        endLine = input.find('\n', startLine);
    }
    if (input.find("CAP") != std::string::npos)
    {
        if (checkLogin(client))
            //send welcome message.
            std::cout << "Client: " << client.nick << " connected.\n";
        else
            //send failed to connect, but every single command send his message.
            std::cout << "Client: " << client.fd << " failed to connect.\n";
    }
}

void        Commands::execCmd( const std::string & command, const std::string & argument, Client & client, Server & server )
{
    _CMD    cmd;

    cmd = strToCmd(command);
    if (cmd == MAX_CMD)
    {
        //command not founnd.
        std::cout << "Command " << command << " not found.\nArguments: " << argument << std::endl;
    }
    else
        commands[cmd].exec(argument, client, server);
}

//All commands of theserver.

void        Commands::execCap( const std::string & argument, Client & client, Server & server )
{
    (void)argument;
    (void)client;
    (void)server;
    std::cout << "Processing CAP\n";
}

void        Commands::execPass( const std::string & argument, Client & client, Server & server )
{
    //Check that pass it's ok.
    std::cout << "Checking pass... " << argument << std::endl;
    if (argument == server.getPassword())
    {
        std::cout << "Pass ok.\n";
        client.authpass = true;
    }
    else
        std::cout << "Wrong pass: " << server.getPassword() << std::endl;
}

void Commands::execNick( const std::string & argument, Client & client, Server & server )
{
    (void)server;
    //Check that nick is not used.
    // else return user change name error.
    std::cout << "Client: " << client.fd << " changed Nick from: " << client.nick;
    client.nick = argument;
    std::cout << " to: " << client.nick << std::endl;
}

void Commands::execUser( const std::string & argument, Client & client, Server & server )
{
    (void)server;
    std::string::size_type space, colon;
    space = argument.find(" ");
    colon = argument.find(":");
    if (space == std::string::npos || colon == std::string::npos)
        //error on user get user o realname.
        std::cout << "Error parsing user or realname.\n";
    else
    {
        client.user = argument.substr(0, space);
        client.realName = argument.substr(colon + 1, argument.size());
        //changed user and realname.
        std::cout << "User: " << client.user << " Realname: " << client.realName << std::endl;
    }
}