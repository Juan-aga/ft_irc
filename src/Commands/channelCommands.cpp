#include "Commands.hpp"
#include "Server.hpp"
#include <cstddef>
#include <iostream>

//Operators commands:
//	KICK
//	INVITE
//	TOPIC
//	MODE

//we have to add this channels mode:
//  Client Limit Channel Mode   +l
//  Invite-Only Channel Mode    +i
//  Key Channel Mode            +k
//  Protected Topic Mode        +t

//clients mode only:
//  Operator:	prefix @		+o
//  regular :	prefix +		+v

//split
static std::vector<std::string> splitString(const std::string& input, char delimiter) {
	std::vector<std::string> parts;

	size_t pos = 0;
	std::string token;
	std::string str = input;

	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		parts.push_back(token);
		str.erase(0, pos + 1);
	}
	parts.push_back(str);

	return parts;
}

// static int 	ft_isnumber(const std::string & num)
// {
// 	for (size_t i = 0; i < num.size(); i++)
// 	{
// 		if (!isdigit(num[i]))
// 			return 0;
// 	}
// 	return 1;
// }



void		Commands::execMode(const std::string & parameter, Client * client, Server & server)
{
	std::vector<std::string> parameters;
	parameters.push_back(parameter);
	(void)client;
	(void)server;
	// (void)parameter;
	std::cout << parameter << std::endl;
	parameters = splitString(parameter, ' ');
	if (parameters[0] == client->nick)
	{
		std::cout << "server MODE" << std::endl;
		//server MODE
	}
	else if (parameters[0][0] == '#') //
	{
		if (!server.getChannelByName(parameters[0])->validName(parameters[0]) || server.getChannelByName(parameters[0]) == NULL)
			Response::createReply(ERR_NOSUCHCHANNEL).From(server).To(*client).Command("MODE").Trailer("Not such channel").Send();
		else if (parameter[1] && (parameters[1].size() == 2 && parameters[1][0] == '+'))
		{
			if (parameters[1] == "+i")
			{
				server.getChannelByName(parameters[0])->inviteOnly = true;
				std::cout << "channel MODE inviteOnly " << server.getChannelByName(parameters[0])->inviteOnly << std::endl;
			}
			else if (parameters[1] == "+t")
			{
				server.getChannelByName(parameters[0])->opTopic = true;
				std::cout << "channel MODE inviteOnly " << server.getChannelByName(parameters[0])->inviteOnly << std::endl;
			}
			else if (parameters[1] == "+k")
			{
				if (parameters[2] == "")
					Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send();
				else
				{
					server.getChannelByName(parameters[0])->password = parameters[2];
					std::cout << "channel MODE password " << server.getChannelByName(parameters[0])->password << std::endl;
				}
			}
			else if (parameters[1] == "+l")
			{
				//std::cout << "channel MODE limit client" << std::endl;
				if (parameters[2] == "")
					Response::createReply(ERR_NEEDMOREPARAMS).From(server).To(*client).Command("MODE").Trailer("Need more params").Send();
				else
					server.getChannelByName(parameters[0])->clientLimit = std::atoi(parameters[2].c_str());
			}
			else
				std::cout << "channel MODE error" << std::endl;
			std::cout << "channel MODE flag " << parameters[1] << std::endl;
		}

		//remove MODE
		else if (parameter[1] && (parameters[1].size() == 2 && parameters[1][0] == '-'))
		{
			if (parameters[1] == "-i")
				server.getChannelByName(parameters[0])->inviteOnly = false;
			else if (parameters[1] == "-t")
				server.getChannelByName(parameters[0])->opTopic = false;
			else if (parameters[1] == "-k")
				server.getChannelByName(parameters[0])->password = "";
			else if (parameters[1] == "+l")
				server.getChannelByName(parameters[0])->clientLimit = 0;
			else
				std::cout << "channel MODE error" << std::endl;
			std::cout << "channel MODE flag " << parameters[1] << std::endl;
		}
		//chanel MODE
	}
	else
		std::cout << "void MODE" << std::endl;
	//std::cout << "[!]/MODE param1: " << parameters[1] << std::end
