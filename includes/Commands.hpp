#pragma once

#include "Client.hpp"
#include <map>

class Commands
{
    private:
        enum    _CMD
        {
            PASS,
            MAX_CMD
        };
        typedef void (*cmdFunction)(const std::string &, Client &);
        typedef struct s_commands
        {
            cmdFunction exec;
        } t_commands;
        t_commands  commands[MAX_CMD];

        std::map<std::string, _CMD> commandMap;

        Commands::_CMD  strToCmd( const std::string & cmd );

        static void execPass( const std::string & argument, Client & client );

    public:
        Commands( void );
        ~Commands( void );

        void    execCmd( const std::string & command, const std::string & argument, Client & client );

};