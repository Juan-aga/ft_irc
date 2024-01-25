#pragma once

#include "Client.hpp"
#include "Utils.hpp"

class Server;


class Commands
{
    private:
        enum    _CMD
        {
            CAP,
            PASS,
            NICK,
            USER,
            JOIN,
            MAX_CMD
        };
        typedef void (*cmdFunction)(const std::string &, Client &, Server &);
        typedef struct s_commands
        {
            cmdFunction exec;
        } t_commands;
        t_commands  commands[MAX_CMD];

        std::map<std::string, _CMD> commandMap;

        Commands::_CMD  strToCmd( const std::string & cmd );

        bool        checkLogin( Client & client );

        static void execCap( const std::string & argument, Client & client, Server & server );
        static void execPass( const std::string & argument, Client & client, Server & server );
        static void execNick( const std::string & argument, Client & client, Server & server );
        static void execUser( const std::string & argument, Client & client, Server & server );
        static void execJoin( const std::string & argument, Client & client, Server & server );

    public:
        Commands( void );
        ~Commands( void );

        void    processInput( const std::string & input, Client & client, Server & server );
        void    execCmd( const std::string & command, const std::string & argument, Client & client, Server & server );

};