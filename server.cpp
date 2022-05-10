#include "server.hpp"

Server::Server(void) {
	return ;
}

Server::~Server(void) {
	return ;
}

void Server::setup() {
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    this->_addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    this->_addrServer.sin_family = AF_INET;
    this->_addrServer.sin_port = htons(30003);

	bind(this->_serverSocket, (const struct sockaddr *)&this->_addrServer, sizeof(this->_addrServer));
    std::cout << "bind : " << this->_serverSocket << std::endl;
    listen(this->_serverSocket, 5);
    std::cout << "listening at : " << ntohs(this->_addrServer.sin_port) << std::endl;
	this->_fds[0].fd = this->_serverSocket;
	this->_fds[0].events = POLLIN;
}

int Server::addUser(int i) {
	int len = this->_wlcmsg.size();
    const char *msg = this->_wlcmsg.c_str();
	int socketClient;
	struct sockaddr_in addrClient;

    socklen_t csize = sizeof(addrClient);
    socketClient = accept(this->_serverSocket, (struct sockaddr *)&addrClient, &csize);
    std::cout << "USER: [" << socketClient << "] connected." << std::endl;
	this->_fds[i].fd = socketClient;
	this->_fds[i].events = POLLIN;
	send(this->_fds[i].fd, msg, len, 0);
}

void Server::servListen(int i) {
	User user;
	for(int x = 1; x < i; x++){
        	if(this->_fds[x].revents & POLLIN){
            	if(recv(this->_fds[x].fd, &user, sizeof(User), 0) == 0){
					std::cout << "USER: [" << this->_fds[x].fd << "] disconnected." << std::endl;
					close(this->_fds[x].fd);
				}
				else 
            		std::cout << user.msg << std::endl;
        	}
		}
}