#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <string>
#include <vector>
#include <thread>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <list>

typedef struct User{
    int     len;
    char    msg[500];
} User;

typedef struct clients{
	struct sockaddr_in addrClient;
	socklen_t csize;
	int socket;
	int nb_msg;
	std::string channel;
}clients;

typedef struct channel{
	int nb_client;
	std::string name;
}channel;

class Server{
	public:
		Server();
		~Server();

		void setup();
		void addUser();
		void servListen(std::list<pollfd>::iterator it);
		void build_fds();
		void update_revents();
		void display_fds();
		bool channel_open(std::string channel_name);
		void user_left(std::string channel_name);
		struct pollfd *get_fds();

		std::list<pollfd> get_lfds();
	private:
		struct pollfd _fds[100];
		struct sockaddr_in _addrServer;

		int _clients;
		int _serverSocket;

		std::string _wlcmsg = "Welcome to our IRC ! enter a channel ";
		std::list<pollfd> _lfds;
		std::list<std::string> _channel;
		std::list<clients> _inf_clients;
		std::list<channel> _in_channel;
};

#endif //SERVER_H
