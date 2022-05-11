#include "server.hpp"


int main()
{
	Server server;

	server.setup();

	int i = 1;
    while(1)
    {
		int r = poll(server._fds, i, 10);
		if(r){
			if(server._fds[0].revents & POLLIN){
				server.addUser(i);
				i++;
			}
			server.servListen(i);
		}
	}
    return 0;
}