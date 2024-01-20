#include "Commands.hpp"

Commands::Commands( void )
{
    commandMap["PASS"] = PASS;

    commands[PASS].exec = &execPass;
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

void        Commands::execCmd( const std::string & command, const std::string & argument, Client & client )
{
    _CMD    cmd;

    cmd = strToCmd(command);
    if (cmd == MAX_CMD)
    {
        //command not founnd.
        std::cout << "Command " << command << " not found.\nARguments:" << argument << std::endl;
    }
    else
        commands[cmd].exec(argument, client);
}

void        Commands::execPass( const std::string & argument, Client & client )
{
    //Check that pass it's ok.
    std::cout << "Checking pass... " << argument << std::endl;
    client._authpass = true;
}