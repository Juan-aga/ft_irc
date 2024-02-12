#include "Server.hpp"

static std::string	botRandom()
{
	srand(time(NULL)); // initializes the random number generator with a seed value based on the current time
	int index = rand() % 10 + 1; // number between 1 and 10

	switch (index)
	{
		case 1: return("Wearing headphones for just an hour could increase the bacteria in your ear by 700 times.");
		case 2: return("Google images was literally created after Jennifer Lopez wore that infamous dress at the 2000 Grammys");
		case 3: return("Los Angeles' full name is 'El Pueblo de Nuestra Senora la Reina de los Angeles de Porciuncula'");
		case 4: return("Tigers have striped skin, not just striped fur.");
		case 5: return("Like fingerprints, everyone's tongue print is different.");
		case 6: return("Cat urine glows under a black-light.");
		case 7: return("A shrimp's heart is in its head.");
		case 8: return("The Spice Girls were originally a band called Touch.");
		case 9: return("The unicorn is the national animal of Scotland");
		case 10: return("In 2014, there was a Tinder match in Antarctica");
	}
	return ("");
}

std::string	Server::bot(std::string bot_cmd)
{
	// List of valid commands
	std::string	validCmds[5] = {
		":HELP",
		":HOUR",
		":RANDOM",
		":COMMANDS",
		":GAME",
		};

	// Convert the command to uppercase
	for (size_t i = 0; i < bot_cmd.size(); i++)
		bot_cmd[i] = std::toupper(bot_cmd[i]);
	
	// Check if the command is valid
	int index = 0;
	while (index < 5)
	{
		if (bot_cmd == validCmds[index])
			break;
		index++;
	}
	// Return the response
	switch (index + 1)
	{
		case 1: return("Bot commands-> :HOUR, :RANDOM, :COMMANDS, :GAME.");
		case 2: return(getTime());
		case 3: return(botRandom());
		case 4: return("Commands available in the server: cap, pass, nick, user, quit, join, privmsg, KILLSERVER, topic, part, kick, mode, invite, who, whois.");
		case 5: return("You lost the game!");
		default: return("Invalid request, ask ':HELP' for more infos");
	}
}