#include "Utils.hpp"
#include "Server.hpp"

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
	signal(SIGQUIT, SIG_IGN);
}