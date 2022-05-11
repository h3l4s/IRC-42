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

typedef struct User{
    int     len;
    char    msg[500];
} User;

typedef struct to_send{
    int     *socketClient;
    int     thread;
} To_send;

typedef struct clients{
	struct sockaddr_in addrClient;
	socklen_t csize;
	int socket;
}clients;

class Server{
	public:
		Server();
		~Server();
		void setup();
		void addUser(int i);
		void servListen(int i);
		struct pollfd *get_fds();
	private:
		struct pollfd _fds[100];
		int _clients;
		std::vector<clients> _sclients;
		std::string _wlcmsg = "Welcome to our IRC ! :o";
		int _serverSocket;
		struct sockaddr_in _addrServer;
};

#endif //SERVER_H
