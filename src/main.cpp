#include "../inc/server.hpp"
#include <stdlib.h>
#include <signal.h>

int port = 0;
int kill_server = 0;

void	inthandler(int sig)
{
	(void)sig;
	std::cout << "Server is close !\n";
	close(port);
	kill_server = 1;
}

bool	check_port(char *str)
{
	int i = 0;

	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return false;
		i++;
	}
	return true;
}
int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cout << "please enter a port and a password.\n";
		exit(0);
	}
	if (check_port(argv[1]) == false)
	{
		std::cout << "Only numbers is accepted for the port\n";
		exit(0);
	}
	Server server(std::atoi(argv[1]), argv[2]);
	std::list<pollfd> temp_lfds = server.get_lfds();
	std::list<pollfd>::iterator begin = temp_lfds.begin();

	signal(SIGINT, inthandler);
	port = std::atoi(argv[1]);
    while(1 && kill_server == 0)
    {
		temp_lfds = server.get_lfds();
		begin = temp_lfds.begin();
		server.update_revents();
		if(poll(server.get_fds(), server.get_lfds().size(), 1)) 
        {
			if(server.get_fds()[0].revents & POLLIN)
				server.addUser();
			temp_lfds = server.get_lfds();
			begin = temp_lfds.begin();
			begin++;
			for( std::list<pollfd>::iterator it = begin; it != temp_lfds.end(); it++){
				server.servListen(it);
			}
		}
	}
    return 0;
}