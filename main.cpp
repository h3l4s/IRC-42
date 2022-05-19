#include "server.hpp"


int main()
{
	Server server;
	std::list<pollfd> temp_lfds = server.get_lfds();
	std::list<pollfd>::iterator begin = temp_lfds.begin();
	
    while(1)
    {
		temp_lfds = server.get_lfds();
		begin = temp_lfds.begin();
		server.update_revents();
		pollfd *tmp = server.get_fds();
		if(poll(server.get_fds(), server.get_lfds().size(), 1)) 
        {
			if(tmp[0].revents & POLLIN)
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