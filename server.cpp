#include "server.hpp"

Server::Server(void) : _clients(0) 
{
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    this->_addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    this->_addrServer.sin_family = AF_INET;
    this->_addrServer.sin_port = htons(30013);
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

Server::~Server(void) {
	return ;
}


void Server::addUser() 
{
	if(this->_clients == 9)
		return ;
	int len = this->_wlcmsg.size();
    const char *msg = this->_wlcmsg.c_str();
	clients new_cli;
    struct pollfd new_fd;

    new_cli.csize = sizeof(new_cli.addrClient);
    new_cli.socket = accept(this->_serverSocket, (struct sockaddr *)&new_cli.addrClient, &new_cli.csize);
    new_cli.nb_msg = 0;
    std::cout << "USER[" << new_cli.socket << "]->[" << inet_ntoa(new_cli.addrClient.sin_addr) <<"] connected." << std::endl;
	new_fd.fd = new_cli.socket;
	new_fd.events = POLLIN;
	send(new_fd.fd, msg, len, 0);
	this->_clients++;
    this->_lfds.push_back(new_fd);
    this->_inf_clients.push_back(new_cli);
    build_fds();
	return ;
}

bool Server::channel_open(std::string channel_name)
{
    for (std::list<std::string>::iterator beg = this->_channels.begin(); beg != this->_channels.end(); beg++)
    {
        if (*beg == channel_name)
        {    
            std::list<channel>::iterator channel = this->_mb_count.begin();
            while ( channel->name != channel_name )
            {
                channel++;
            }
            channel->nb_client++;
            return true;
        }    
    }
    return false;
}

void Server::user_left(std::string channel_name)
{
      
    std::list<channel>::iterator channel = this->_mb_count.begin();
    while ( channel->name != channel_name )
        channel++;
    channel->nb_client--;
    if (channel->nb_client == 0){
        std::cout << "CHANNEL[" <<  channel_name << "] has been closed.\n";
        this->_mb_count.erase(channel);
    }
    return ;
}

void Server::servListen(std::list<pollfd>::iterator it) 
{
	User user;
    if(it->revents & POLLIN){
        if(recv(it->fd, &user, sizeof(User), 0) == 0)
        {
            std::list<clients>::iterator it_cli = this->_inf_clients.begin();   
            while (it_cli->socket != it->fd)
                it_cli++;
			std::cout << "USER[" << it->fd << "] disconnected." << std::endl;
			close(it->fd);
			this->_clients--;
            std::list<pollfd>::iterator beg = this->_lfds.begin();
            while (beg->fd != it->fd)
                beg++;
            this->_lfds.erase(beg);
            user_left(it_cli->channel);
            this->_inf_clients.erase(it_cli);
            build_fds();
		}
		else 
        {
            std::list<clients>::iterator it_cli = this->_inf_clients.begin();   
            while (it_cli->socket != it->fd)
                it_cli++;
            if (it_cli->nb_msg == 0){
                it_cli->channel.assign(user.msg);
                std::cout << "CHANNEL[#" << it_cli->channel << "] created.\n";
                if (this->_channels.empty() == true){
                    this->_channels.push_back(it_cli->channel);
                    channel channel;
                    channel.name = it_cli->channel;
                    channel.nb_client = 1;
                    this->_mb_count.push_back(channel);
                }
                else 
                {
                    if (channel_open(it_cli->channel) == false){
                        this->_channels.push_back(it_cli->channel);
                        channel channel;
                        channel.name = it_cli->channel;
                        channel.nb_client = 1;
                        this->_mb_count.push_back(channel);
                    }  

                }
                send(it->fd, "channel successfully created.", 30, 0);
                it_cli->nb_msg++;
            }
            else
       	        std::cout << "USER[" << it_cli->socket << "](#" << it_cli->channel << "): " << user.msg << std::endl;
        }
        display_fds();
    }
return ;
}

struct pollfd *Server::get_fds( void )
{
	return this->_fds;
}

std::list<pollfd> Server::get_lfds( void )
{
    return this->_lfds;
}

void Server::build_fds(void) 
{
    int i = 0;
    for(std::list<pollfd>::iterator it = this->_lfds.begin(); it != this->_lfds.end(); it++)
    {
        this->_fds[i] = *it;
        i++;
    }
    return ;
}

void Server::update_revents( void ) 
{
    std::list<pollfd>::iterator it = this->_lfds.begin();
    for(int i = 0; i < this->_lfds.size(); i++) 
    {
        it->revents = this->_fds[i].revents;
        it->events = this->_fds[i].events;
        it++;
    }
    return ;
}

void Server::display_fds( void )
{
    std::list<channel>::iterator it = this->_mb_count.begin();
    std::list<channel>::iterator ite = this->_mb_count.end();

    while(it != ite)
    {
        std::cout << "CHANNEL[#" << it->name << "]: " << it->nb_client << " member(s)." << std::endl;
        it++;
    }
    return ;
}