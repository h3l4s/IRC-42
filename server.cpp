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
    this->_addrServer.sin_port = htons(30002);
    struct pollfd lserver;

	bind(this->_serverSocket, (const struct sockaddr *)&this->_addrServer, sizeof(this->_addrServer));
    std::cout << "bind : " << this->_serverSocket << std::endl;
    listen(this->_serverSocket, 5);
    std::cout << "listening at : " << ntohs(this->_addrServer.sin_port) << std::endl;
	lserver.fd = this->_serverSocket;
	lserver.events = POLLIN;
    this->_lfds.push_back(lserver);
    build_fds();
	return ;
}

void Server::addUser() {
	if(this->_clients == 99)
		return ;
	int len = this->_wlcmsg.size();
    const char *msg = this->_wlcmsg.c_str();
	clients new_cli;
    struct pollfd new_fd;

    new_cli.csize = sizeof(new_cli.addrClient);
    new_cli.socket = accept(this->_serverSocket, (struct sockaddr *)&new_cli.addrClient, &new_cli.csize);
    std::cout << "USER: [" << new_cli.socket << "]->[" << inet_ntoa(new_cli.addrClient.sin_addr) <<"] connected." << std::endl;
	new_fd.fd = new_cli.socket;
	new_fd.events = POLLIN;
	send(new_fd.fd, msg, len, 0);
	this->_clients++;
    this->_lfds.push_back(new_fd);
    build_fds();
	return ;
}

void Server::servListen(std::list<pollfd>::iterator it) {
	User user;
    if(it->revents & POLLIN){
        if(recv(it->fd, &user, sizeof(User), 0) == 0){
			std::cout << "USER: [" << it->fd << "]->[" << "00.00.00.00" <<"] disconnected." << std::endl;
			close(it->fd);
			this->_clients--;
            std::list<pollfd>::iterator beg = this->_lfds.begin();
            while (beg->fd != it->fd)
                beg++;
            this->_lfds.erase(beg);
            build_fds();
		}
		else 
       	    std::cout << "[USER " << it->fd << "]: " << user.msg << std::endl;
    }
return ;
}

struct pollfd *Server::get_fds(){
	return this->_fds;
}

std::list<pollfd> Server::get_lfds(){
    return this->_lfds;
}

void Server::build_fds(void) {
    int i = 0;
    for(std::list<pollfd>::iterator it = this->_lfds.begin(); it != this->_lfds.end(); it++){
        this->_fds[i] = *it;
        i++;
    }
    return ;
}

void Server::update_revents() {
    std::list<pollfd>::iterator it = this->_lfds.begin();
    for(int i = 0; i < this->_lfds.size(); i++) {
        it->revents = this->_fds[i].revents;
        it->events = this->_fds[i].events;
        it++;
    }
    return ;
}