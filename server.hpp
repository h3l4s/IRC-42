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

# define JOIN 1
# define QUIT 2

typedef struct User{
    int     len;
    char    msg[500];
} User;

typedef struct clients{
	struct sockaddr_in addrClient;
	socklen_t csize;
	int socket;
	int nb_msg;
	std::string username;
	std::string password;
	std::string name;
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

		void addUser();
		void servListen(std::list<pollfd>::iterator it);
		void update_revents();
		struct pollfd *get_fds();
		std::list<pollfd> get_lfds();
		
	private:
		struct pollfd _fds[100];
		struct sockaddr_in _addrServer;

		void build_fds();
		void display_fds();
		void setup_username( std::string nickname, std::list<clients>::iterator it_cli);
		void setup_password( std::string password, std::list<clients>::iterator it_cli);
		void user_left( std::list<pollfd>::iterator it );
		bool channel_open(std::string channel_name);
		void channel_empty(std::string channel_name);
		void create_channel(int user, std::list<clients>::iterator it_cli, std::string msg);
		
		int _clients;
		int _serverSocket;

		std::string _wlcmsg = "Welcome to our IRC ! enter a channel ";
		std::list<pollfd> _lfds;
		std::list<clients> _user_data;
		std::list<channel> _channel_data;
};

#endif //SERVER_H
