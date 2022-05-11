#include "server.hpp"

Server::Server(void) : _clients(0) {
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
	return ;
}

void Server::addUser(int i) {
	if(this->getClients() == 99)
		return ;
	int len = this->_wlcmsg.size();
    const char *msg = this->_wlcmsg.c_str();
	clients new_cli;

    new_cli.csize = sizeof(new_cli.addrClient);
    new_cli.socket = accept(this->_serverSocket, (struct sockaddr *)&new_cli.addrClient, &new_cli.csize);
    std::cout << "USER: [" << new_cli.socket << "]->[" << inet_ntoa(new_cli.addrClient.sin_addr) <<"] connected." << std::endl;
	this->_fds[i].fd = new_cli.socket;
	this->_fds[i].events = POLLIN;
	send(this->_fds[i].fd, msg, len, 0);
	this->addClients();
	this->_sclients.push_back(new_cli);
	return ;
}

void Server::servListen(int i) {
	User user;
	for(int x = 1; x < i; x++){
        	if(this->_fds[x].revents & POLLIN){
            	if(recv(this->_fds[x].fd, &user, sizeof(User), 0) == 0){
					std::cout << "USER: [" << this->_fds[x].fd << "]->[" << this->getIP(x -1) <<"] disconnected." << std::endl;
					close(this->_fds[x].fd);
					this->removeClients(x - 1);

				}
				else 
            		std::cout << user.msg << std::endl;
        	}
		}
	return ;
}

void Server::addClients(){
	this->_clients++;
	return ;
}

void Server::removeClients(int i){
	this->_clients--;
	this->_sclients.erase(this->_sclients.begin() + 6);
	return ;
}

int Server::getClients(){
	return this->_clients;
}

char *Server::getIP(int i){
	return (inet_ntoa(this->_sclients[i].addrClient.sin_addr));
}