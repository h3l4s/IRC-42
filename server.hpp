#ifndef SERVER_H
#define SERVER_H
#include "utils.hpp"

class Server{
	public:
		struct pollfd _fds[100];
		std::string _wlcmsg = "Welcome to our IRC ! :o";
		int _serverSocket;
		struct sockaddr_in _addrServer;
		Server();
		~Server();
		void setup();
		int addUser(int i);
		void servListen(int i);


	private:
};

#endif //SERVER_H