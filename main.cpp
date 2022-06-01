#include "server.hpp"
#include <stdlib.h>
#include <signal.h>

int port = 0;

void	inthandler(int sig)
{
	std::cout << "Server is close !\n";
	close(port);
	exit(1);
}

int main(int argc, char **argv)
{
	Server server(std::atoi(argv[1]), argv[2]);
	std::list<pollfd> temp_lfds = server.get_lfds();
	std::list<pollfd>::iterator begin = temp_lfds.begin();

	signal(SIGINT, inthandler);
	port = std::atoi(argv[1]);
	
    while(1)
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