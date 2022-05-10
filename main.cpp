#include "server.hpp"


int main()
{
	Server server;

	server.setup();

    User user;
	int i = 1;
	server._fds[0].fd = server._serverSocket;
	server._fds[0].events = POLLIN;
    while(1)
    {
		int e = poll(server._fds, i, 10);
		if(server._fds[0].revents & POLLIN){
			server.addUser(i);
			i++;
		}
		server.servListen(i);
	}
    return 0;
}