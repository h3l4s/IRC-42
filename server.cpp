#include "server.hpp"

Server::Server(void) {
	return ;
}

Server::Server( int port, std::string password ) : _clients(0), _passwd(password)
{
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    this->_addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    this->_addrServer.sin_family = AF_INET;
    this->_addrServer.sin_port = htons(port);
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
	clients new_cli;
    struct pollfd new_fd;

    new_cli.csize = sizeof(new_cli.addrClient);
    new_cli.socket = accept(this->_serverSocket, (struct sockaddr *)&new_cli.addrClient, &new_cli.csize);
    new_cli.nb_msg = 0;
	new_cli.oper = 0;
	new_cli.invisible = 0;
    std::cout << "USER[" << new_cli.socket << "]->[" << inet_ntoa(new_cli.addrClient.sin_addr) <<"] connected." << std::endl;
	new_fd.fd = new_cli.socket;
	new_fd.events = POLLIN;
	this->_clients++;
    this->_lfds.push_back(new_fd);
    this->_user_data.push_back(new_cli);
    build_fds();
	return ;
}

bool Server::channel_open(std::string channel_name, int user)
{
    for (std::list<channel>::iterator beg = this->_channel_data.begin(); beg != this->_channel_data.end(); beg++)
    {
        if (beg->name == channel_name)
        {    
            beg->nb_client++;
            beg->client_socket.push_back(user);
            return true;
        }    
    }
    return false;
}

void Server::channel_empty(std::string channel_name)
{
    std::list<channel>::iterator it = this->_channel_data.begin();
    std::list<channel>::iterator ite = this->_channel_data.end();
    while ( it->name != channel_name && it != ite)
        it++;
    if (it != ite){
        it->nb_client--;
        if (it->nb_client == 0){
            std::cout << "channel " <<  channel_name << " is close \n";
            this->_channel_data.erase(it);
        }
    }
    return ;
}

void Server::create_channel(int user, std::list<clients>::iterator it_cli, std::string channel_name)
{
    it_cli->channel.push_back(channel_name);
    if (this->_channel_data.size() == 0){
        std::cout << "channel " << channel_name << " creer\n";
        channel channel;
        channel.name = channel_name;
        channel.nb_client = 1;
        channel.client_socket.push_back(user);
        this->_channel_data.push_back(channel);
    }
    else {
        if (channel_open(channel_name, user) == false){
            std::cout << "channel " << channel_name << " existe\n";
            channel channel;
            channel.name = channel_name;
            channel.nb_client = 1;
            channel.client_socket.push_back(user);
            this->_channel_data.push_back(channel);
        }  
    }
}

void Server::user_left(std::list<pollfd>::iterator it)
{
    std::list<clients>::iterator it_cli = this->_user_data.begin();   
    while (it_cli->socket != it->fd)
        it_cli++;
    std::cout << "USER[" << it->fd << "] disconnected." << std::endl;
    close(it->fd);
    this->_clients--;
    std::list<pollfd>::iterator beg = this->_lfds.begin();
    while (beg->fd != it->fd)
        beg++;
    this->_lfds.erase(beg);
    //channel_empty(it_cli->channel);
    this->_user_data.erase(it_cli);
    build_fds();
    return ;
}

void Server::setup_username( std::string username, std::list<clients>::iterator it_cli, int first )
{
    //std::cout << "test = " << "|"<< username << "|"  << "   first " << first << std::endl;
    //std::cout << "test = " << username.size() << std::endl;
    /* if (username.size() > 14)
    {
        send(it_cli->socket, "Username is under 9 character.", 30, 0);
        return ;
    } */
    for(std::list<clients>::iterator to_send = this->_user_data.begin(); to_send != this->_user_data.end(); to_send++)
    {
        if (username == to_send->username)
        {
            username = username + "_";
            break ;
        }
    }
    if (it_cli->username.empty() == false)
        it_cli->username.clear();
    it_cli->username = username;
    return ;
}

void Server::setup_password( std::string password, std::list<clients>::iterator it_cli )
{
    if (it_cli->password.empty() == false)
        it_cli->password.clear();
    it_cli->password.insert(it_cli->password.begin(), password.begin()+6, password.end());
    return ;
}

void Server::delete_clrf(std::string temp)
{
    std::string first;
    std::string::iterator it = temp.begin();
    std::string::iterator position = temp.begin();

    while (it != temp.end())
    {
        if (*it == '\r')
        {
            first.assign(position, it);
            this->cmd.push_back(first);
            first.clear();
            position = it;
            position = position + 2;
        }
        it++;
    }
    for (std::vector<std::string>::iterator it = this->cmd.begin(); it != this->cmd.end(); it++)
    {
        std::cout << "cmd = |" << *it << "|\n";
    }

    return ;
}

void Server::setup_host( std::string host, std::list<clients>::iterator it_cli )
{
    int zero = 0;
    std::string::iterator it = host.begin();
    std::string::iterator position = host.begin();
    
    while (it != host.end())
    {
        if (*it == ' ')
        {    
            zero++;
            position = it;
            position++;
        }
        it++;
        if (zero == 2)
            break;
    }
    while (it != host.end())
    {
        if (*it == ' ')
            break;
        it++;
    }
    if (it_cli->host.empty() == false)
        it_cli->host.clear();
    it_cli->host.assign(position, it);
    return ;
}

std::string Server::cut_word_space( std::string to_cut, std::string::iterator it )
{
    std::string after_cut;
    std::string::iterator it_space = it;
    while (*it_space != ' ')
        it_space++;
    
    after_cut.assign(it, it_space);
    std::cout << "after = |" << after_cut << "|\n";
    return after_cut;
}

void Server::delete_channel(std::list<clients>::iterator it_cli, std::string channel_name)
{
    std::list<std::string>::iterator it = it_cli->channel.begin();
    while (it != it_cli->channel.end()){
        if (*it == channel_name){
            it->clear();
            it_cli->channel.erase(it);
            return ;
        }
        it++;
    }
    return ;
}

void Server::commandPART(std::list<clients>::iterator it_cli, std::string it)
{
    //int position = -1;
    std::list<int>::iterator to_del;
    //if (it.find('#') != std::string::npos)
    //  position = it.find('#');
    //else if (it.find('&') != std::string::npos)
    //    position = it.find('&');
    std::string channel_name;
    //if (position != -1)
    channel_name = it;
    it = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " PART " + it + "\r\n";
    for(std::list<channel>::iterator to_send = this->_channel_data.begin(); to_send != this->_channel_data.end(); to_send++)
    {
        if (channel_name == to_send->name)
        {
            std::cout << "priv msg channel = |" << it << "|\n";
            for (std::list<int>::iterator socket_in_channel = to_send->client_socket.begin(); socket_in_channel != to_send->client_socket.end(); socket_in_channel++){
                if (it_cli->socket == *socket_in_channel )
                    to_del = socket_in_channel;
                else
                    send(*socket_in_channel, it.c_str() , it.size(), 0);
            }
            to_send->client_socket.erase(to_del);
        }
    }
    delete_channel(it_cli, channel_name);
    return ;
}

void Server::commandJOIN( std::list<clients>::iterator it_cli, std::string it )
{
    std::string channel_name;
    channel_name = it;
    create_channel(it_cli->socket, it_cli, channel_name);
    it = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " JOIN " + it + "\r\n";
    for(std::list<channel>::iterator to_send = this->_channel_data.begin(); to_send != this->_channel_data.end(); to_send++)
    {
        if (channel_name == to_send->name)
        {
            std::cout << "priv msg channel = |" << it << "|\n";
            for (std::list<int>::iterator socket_in_channel = to_send->client_socket.begin(); socket_in_channel != to_send->client_socket.end(); socket_in_channel++){
                    if (it_cli->socket != *socket_in_channel)
                        send(*socket_in_channel, it.c_str() , it.size(), 0);
					std::cout << "socket_in_channel" <<  *socket_in_channel << std::endl;
            }
        }
    }
}

void Server::commandNICK( std::list<clients>::iterator it_cli, std::string it )
{
    std::string old_username = it_cli->username;
    setup_username(it, it_cli, it.find("NICK", 0, 5));
	std::cout << "it_cli USERNAME: " << it_cli->username << std::endl;
    std::string output = ":" + old_username + " NICK " + it_cli->username + "\r\n";
    send(it_cli->socket, output.c_str() , output.size(), 0);
}

bool Server::is_in_the_channel(std::list<std::string> channel, std::string channel_name)
{
    for (std::list<std::string>::iterator it = channel.begin(); it != channel.end(); it++){
        if (*it == channel_name)
            return true;
    }
    return false;
}

void Server::commandPRIVMSG_channel( std::list<clients>::iterator it_cli, std::string message )
{
	int pos;
	std::string channel_name;
	int valide = 0;
    std::string user_not_found;
	if ((pos = message.find(" ")) != std::string::npos) {
		channel_name = message.substr(0, pos);
	}
    message = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " PRIVMSG " + message;
    for(std::list<channel>::iterator to_send = this->_channel_data.begin(); to_send != this->_channel_data.end(); to_send++)
    {
        if (channel_name == to_send->name)
        {
			valide++;
            std::cout << "priv msg channel = |" << message << "|\n";
            for (std::list<int>::iterator socket_in_channel = to_send->client_socket.begin(); socket_in_channel != to_send->client_socket.end(); socket_in_channel++){
                if (it_cli->socket != *socket_in_channel && is_in_the_channel(it_cli->channel, channel_name) == true)
                    send(*socket_in_channel, message.c_str() , message.size(), 0);
            }
        }
    }
	if (valide == 0)
    {
        user_not_found = ":127.0.0.1 401 " + it_cli->username + " :" + channel_name + " \r\n";
        send(it_cli->socket, user_not_found.c_str() , user_not_found.size(), 0);
    }
    return;
}

void Server::commandPRIVMSG_user( std::list<clients>::iterator it_cli, std::string it )
{
	int pos;
	int valide = 0;
    std::string user_not_found;
	std::string user_to_send;
	if ((pos = it.find(" ")) != std::string::npos) {
		user_to_send = it.substr(0, pos);
	}
    it = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " PRIVMSG " + it;
    for(std::list<clients>::iterator to_send = this->_user_data.begin(); to_send != this->_user_data.end(); to_send++)
    {
        std::cout << "cmd = |" << it << "|\n";
        if (user_to_send == to_send->username){
			std::cout << "to send  boucle = " << user_to_send << " username = " << to_send->username << std::endl;
            send(to_send->socket, it.c_str() , it.size(), 0);
			valide++;
            break ;
        }
		std::cout << "to send  = " << user_to_send << " username = " << to_send->username << std::endl;
    }
	if (valide == 0)
    {
        user_not_found = ":127.0.0.1 401 " + it_cli->username + " :" + user_to_send + " \r\n";
        send(it_cli->socket, user_not_found.c_str() , user_not_found.size(), 0);
    }
    return ;
}

void Server::commandPRIVMSG( std::list<clients>::iterator it_cli, std::string message )
{
    message = message + "\r\n";
    if (message.find("#") != std::string::npos || message.find("&") != std::string::npos)
        commandPRIVMSG_channel(it_cli, message);
    else 
        commandPRIVMSG_user(it_cli, message);
}

void Server::commandNOTICE( std::list<clients>::iterator it_cli, std::string it )
{
    it = it + "\r\n";
    int pos;
	std::string user_to_send;
	user_to_send = cut_word_space(it, it.begin() + 7);
    it = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " " + it;
    for(std::list<clients>::iterator to_send = this->_user_data.begin(); to_send != this->_user_data.end(); to_send++)
    {
        std::cout << "cmd = |" << it << "|\n";
        if (user_to_send == to_send->username){
            std::cout << "to send  boucle = " << user_to_send << " username = " << to_send->username << std::endl;
            send(to_send->socket, it.c_str() , it.size(), 0);
            break ;
        }
        std::cout << "to send  = " << user_to_send << " username = " << to_send->username << std::endl;
    }
    return ;
}

std::string Server::username_with_socket(int socket)
{
    std::list<clients>::iterator it = this->_user_data.begin();

    while (it != this->_user_data.end())
    {
        if (it->socket == socket)
            return it->username;
        it++;
    }
    return NULL;
}

void Server::commandNAME(  std::list<clients>::iterator it_cli )
{

    std::string channel_name;
    std::string user_name;
    std::list<channel>::iterator it = this->_channel_data.begin();
    while (it != this->_channel_data.end())
    {
        channel_name = ":127.0.0.1 322 " + it_cli->username + " :" + it->name + " :\r\n" ;
        send(it_cli->socket, channel_name.c_str() , channel_name.size(), 0);
        for (std::list<int>::iterator it_socket = it->client_socket.begin(); it_socket != it->client_socket.end(); it_socket++)
        {
            user_name = ":127.0.0.1 " + it_cli->username + " :" + username_with_socket(*it_socket) + " \r\n" ;
            send(it_cli->socket, user_name.c_str() , user_name.size(), 0);
            user_name.clear();
        }
        it++;
        channel_name.clear();
    }
}

void Server::commandLIST(  std::string cmd , std::list<clients>::iterator it_cli )
{
    if (this->_channel_data.size() == 0)
        return ;
    std::string channel_name;
    std::string separate_msg = "list of channels : \r\n";
    channel_name.assign(cmd.begin(), cmd.end());
    std::list<channel>::iterator it = this->_channel_data.begin();
    send(it_cli->socket, separate_msg.c_str() , separate_msg.size(), 0);
    while (it != this->_channel_data.end())
    {
        channel_name = ":127.0.0.1 322 " + it_cli->username + " :" + it->name + "\r\n" ;
        it++;
        send(it_cli->socket, channel_name.c_str() , channel_name.size(), 0);
        channel_name.clear();
    }

}

bool Server::is_in_channel(std::string channel, std::list<std::string> channel_list)
{
    std::list<std::string>::iterator it = channel_list.begin();

    while (it != channel_list.end())
    {
        if(*it == channel)
            return true;
        it++;
    }
    return false;
}

void Server::commandQUIT( std::string cmd , std::list<clients>::iterator it_cli, std::list<pollfd>::iterator it)
{
    std::cout << "USER[" << it->fd << "] disconnected." << std::endl;
    cmd = ":" + it_cli->username + "!" + it_cli->host + "@" + it_cli->host + " " + cmd + "\r\n";
    for(std::list<channel>::iterator to_send = this->_channel_data.begin(); to_send != this->_channel_data.end(); to_send++)
    {
        if (is_in_channel(to_send->name, it_cli->channel) == true)
        {
            std::cout << "priv msg channel = |" << cmd << "|\n";
            for (std::list<int>::iterator socket_in_channel = to_send->client_socket.begin(); socket_in_channel != to_send->client_socket.end(); socket_in_channel++){
                if (it_cli->socket != *socket_in_channel)
                    send(*socket_in_channel, cmd.c_str() , cmd.size(), 0);
            }
        }
    }
    close(it->fd);
    this->_clients--;
    std::list<pollfd>::iterator beg = this->_lfds.begin();
    while (beg->fd != it->fd)
        beg++;
    this->_lfds.erase(beg);
    this->_user_data.erase(it_cli);
    build_fds();
}

void Server::commandMODE( std::list<clients>::iterator it_cli, std::string args){
	if(args.find("-i") != std::string::npos){
		it_cli->invisible = 1;
	}
	else if(args.find("-o") != std::string::npos){
		it_cli->oper = 1;
	}
	return ;
}

void Server::servListen(std::list<pollfd>::iterator it) 
{
    char rec_char[500];
    std::string temp;
    int rec;
	std::vector<std::string>::iterator it_cmd;

    if(it->revents & POLLIN){
        //rec_char = strcpy
        for(int i = 0; i < 500; i++){
            rec_char[i] ^= rec_char[i];
        }
        rec = recv(it->fd, &rec_char ,sizeof(rec_char), 0);
        temp.assign(rec_char);
        delete_clrf(temp);
        it_cmd = this->cmd.begin();
        //std::cout << "displaying commands in buffer" << std::endl;
        //std::cout << *it_cmd << std::endl;
        std::list<clients>::iterator it_cli = this->_user_data.begin();  
        while (it_cli->socket != it->fd)
            it_cli++;
		while(it_cmd != this->cmd.end()){
			if(it_cmd->find("USER") != std::string::npos){
				setup_host(*it_cmd, it_cli);
			}
            else
			    parser(*it_cmd, it, it_cli);
			if(it_cmd->find("QUIT") != std::string::npos)
				return ;
            it_cmd->clear();
			it_cmd++;
		}
		if (it_cli->password == this->_passwd && it_cli->nb_msg == 0)
    	{
        	std::string _wlcmsg = ":127.0.0.1 375 " +  it_cli->username + " ::- 127.0.0.1 Message of the day -\r\n";
			std::string _wlcmsg2 = ":127.0.0.1 376 " +  it_cli->username + " ::End of /MOTD command\r\n";
    
        	send(it_cli->socket, _wlcmsg.c_str(), _wlcmsg.size(), 0);
        	send(it_cli->socket, _wlcmsg2.c_str(), _wlcmsg2.size(), 0);
        	it_cli->nb_msg++;
    	}
		if(it_cli->password != "" && it_cli->password != this->_passwd){
            std::string wrong_pass = "error: wrong password. try reconnecting with a correct password.\r\n";
            send(it_cli->socket, wrong_pass.c_str(), wrong_pass.size(), 0);
            //call quit command ? or ask for another password with a custom message
        }
		if(rec == 0)
            user_left(it);
    }
    //std::cout << "TEST = " << *this->cmd.begin() << std::endl; 
    this->cmd.clear();
    
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
    std::list<channel>::iterator it = this->_channel_data.begin();
    std::list<channel>::iterator ite = this->_channel_data.end();

    while(it != ite)
    {
        it++;
    }
    return ;
}

////////////////////////////////////////////////
///////////////////////////////////////////////
//////////////////////////////////////////////
/////////////////////////////////////////////
////////////////////////////////////////////
///////////////////////////////////////////
//////////////////////////////////////////
/////////////////////////////////////////

int Server::no_arg(struct msg msg, std::list<pollfd>::iterator it,
    std::list<clients>::iterator it_cli ){
	std::cout << "appear in no_arg" << std::endl;
    commandQUIT(msg.cmd + " " + msg.args, it_cli, it);
	return 0;
}

int Server::one_arg(struct msg msg, std::list<pollfd>::iterator it,
    std::list<clients>::iterator it_cli ){
	std::cout << "appear in one_arg: " << msg.cmd << std::endl;
    if(msg.cmd.find("PASS") != std::string::npos)
        it_cli->password = msg.args;
    if(msg.cmd.find("NICK") != std::string::npos)
		commandNICK(it_cli, msg.args);
	return 0;
}

int Server::multiple_args(struct msg msg, std::list<pollfd>::iterator it,
    std::list<clients>::iterator it_cli ){
	std::cout << "appear in multiple_args" << std::endl;
	int pos;
	std::string delimiter = ",";
	std::string temp;
	std::string temp2;
	if(msg.cmd.find("JOIN") != std::string::npos){
		if(msg.args.find(delimiter) != std::string::npos){
		while ((pos = msg.args.find(delimiter)) != std::string::npos) { // splitting using spaces
			temp = msg.args.substr(0, pos);
			commandJOIN(it_cli, temp);
    		msg.args.erase(0, pos + delimiter.length());
		}
		commandJOIN(it_cli, msg.args);
		}
		else{
			std::cout << "ELSE" << std::endl;
			commandJOIN(it_cli, msg.args);
		}
	}
	if(msg.cmd.find("PART") != std::string::npos){
		if(msg.args.find(delimiter) != std::string::npos){
		while ((pos = msg.args.find(delimiter)) != std::string::npos) { // splitting using spaces
			temp = msg.args.substr(0, pos);
			commandPART(it_cli, temp);
    		msg.args.erase(0, pos + delimiter.length());
		}
		commandPART(it_cli, msg.args);
		}
		else{
			commandPART(it_cli, msg.args);
		}
	}
	if(msg.cmd.find("PRIVMSG") != std::string::npos){
		if ((pos = msg.args.find(" ")) != std::string::npos) { // splitting using spaces
			temp = msg.args.substr(0, pos);
    		msg.args.erase(0, pos + 1);
		}
		if(temp.find(",") != std::string::npos){
		while ((pos = temp.find(",")) != std::string::npos) { // splitting using spaces
			temp2 = msg.args.substr(0, pos);
			commandPRIVMSG(it_cli, (temp2 + " " + msg.args));
    		temp.erase(0, pos + 1);
		}}
		else
			temp2 = temp;
		commandPRIVMSG(it_cli, temp2 + " " + msg.args);
	}
	if(msg.cmd.find("NAME") != std::string::npos){
		commandNAME(it_cli);
	}
	if(msg.cmd.find("LIST") != std::string::npos){
		if(msg.args.find(",") != std::string::npos){
		while ((pos = msg.args.find(",")) != std::string::npos) { // splitting using spaces
			temp2 = msg.args.substr(0, pos);
			commandLIST(msg.cmd + " " + msg.args, it_cli);
    		msg.args.erase(0, pos + 1);
		}}
		commandLIST(msg.cmd + " " + msg.args, it_cli);
	}
	if(msg.cmd.find("NOTICE") != std::string::npos){
		commandNOTICE(it_cli, msg.cmd + " " + msg.args);
	}
	if(msg.cmd.find("MODE") != std::string::npos){
		commandMODE(it_cli, msg.args);
	}
	return 0;
}

int Server::choose_option(std::string cmd){
	std::string options[14] = { "QUIT", "PASS", "NICK",
	"USER", "OPER", "JOIN", "PART", "MODE", "NOTICE",
	"NAMES", "LIST", "PRIVMSG", "KICK" };
	int i = 0;
	while(i < 13){
		if(options[i] == cmd)
			break;
		i++;
	}
	switch (i){
		case 0:
			return 0; // case single command no arg
		case 1 ... 2:
			return 1; // case single command 1 arg
		case 3 ... 12:
			return 2; // case single command multiple args
	}
	return -1;
}

void Server::global_parsing(std::string s, std::list<pollfd>::iterator it,
    std::list<clients>::iterator it_cli ){
	this->_msg.result[0] = &this->_msg.prefix;
	this->_msg.result[1] = &this->_msg.cmd;
	this->_msg.result[2] = &this->_msg.args;
	std::string delimiter = " ";
	size_t pos = 0;
	int i;

	s[0] == ':' ? i = 0 : i = 1; // checking if a prefix is present
	if(i == 1)
		*this->_msg.result[0] = "";
	while ((pos = s.find(delimiter)) != std::string::npos && i != 2) { // splitting using spaces
		*this->_msg.result[i] = s.substr(0, pos);
    	s.erase(0, pos + delimiter.length());
		i++;
	}
	*this->_msg.result[i] = s;
	i = choose_option(*this->_msg.result[1]);
	if(i == -1){
		std::cout << "error: command not found." << std::endl;
		return ;
	}
	int (Server::*options_ft[4])(struct msg, std::list<pollfd>::iterator, std::list<clients>::iterator) = { &Server::no_arg, &Server::one_arg, &Server::multiple_args };
	(this->*options_ft[i])(this->_msg,it, it_cli);
    return ;
}

int Server::parser(std::string cmd, std::list<pollfd>::iterator it,
    std::list<clients>::iterator it_cli ){
	std::string input;

	input.assign(cmd);
	global_parsing(input, it, it_cli);
	return 0;
}