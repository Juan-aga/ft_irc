#include "Utils.hpp"
#include "Server.hpp"
//for linux
#include <signal.h>

static void	ft_handler_ctrl_c(int sig)
{
	if (sig == SIGINT)
	{
		Server::stopServer();
	}
}

void	ft_init_signals(void)
{
	signal(SIGINT, ft_handler_ctrl_c);
}