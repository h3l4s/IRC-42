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
		int r = poll(server.get_fds(), server.get_lfds().size(), 10);
		server.update_revents();
		if(r) 
        {
			if(begin->revents & POLLIN)
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