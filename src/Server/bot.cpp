#include "Server.hpp"

static std::string	botRandom()
{
	srand(time(NULL)); // initializes the random number generator with a seed value based on the current time
	int index = rand() % 10 + 1; // number between 1 and 10
	
	std::string str;
	switch (index)
	{
		case 1: str = "Wearing headphones for just an hour could increase the bacteria in your ear by 700 times."; break;
		case 2: str = "Google images was literally created after Jennifer Lopez wore that infamous dress at the 2000 Grammys"; break;
		case 3: str = "Los Angeles' full name is 'El Pueblo de Nuestra Senora la Reina de los Angeles de Porciuncula'"; break;
		case 4: str = "Tigers have striped skin, not just striped fur."; break;
		case 5: str = "Like fingerprints, everyone's tongue print is different."; break;
		case 6: str = "Cat urine glows under a black-light."; break;
		case 7: str = "A shrimp's heart is in its head."; break;
		case 8: str = "The Spice Girls were originally a band called Touch."; break;
		case 9: str = "The unicorn is the national animal of Scotland"; break;
		case 10: str = "In 2014, there was a Tinder match in Antarctica"; break;
	}
	return (str);
}

std::string	Server::bot(std::string bot_cmd)
{
	// List of valid commands
	std::string	validCmds[3] = {
		":HELP",
		":HOUR",
		":RANDOM",
		};

	// Convert the command to uppercase
	for (size_t i = 0; i < bot_cmd.size(); i++)
		bot_cmd[i] = std::toupper(bot_cmd[i]);
	
	// Check if the command is valid
	int index = 0;
	while (index < 3)
	{
		if (bot_cmd == validCmds[index])
			break;
		index++;
	}
	// Return the response
	switch (index + 1)
	{
		case 1: return("Ask me ':HOUR' or ':RANDOM'");
		case 2: return(getTime());
		case 3: return(botRandom());
		default: return("Invalid request, ask ':HELP' for more infos");
	}
}