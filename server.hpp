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
	std::list<std::string> channel;
	std::string host;
	bool oper;
	bool invisible;
}clients;

typedef struct channel{
	int nb_client;
	std::string name;
	std::list<int> client_socket;
	std::list<std::string> username;
}channel;

typedef struct msg{
	std::string cmd;
	std::string prefix;
	std::string args;
	std::string *result[3];
} msg;

class Server{
	public:
		Server();
		Server( int port, std::string password );
		~Server();

		void addUser();
		void servListen(std::list<pollfd>::iterator it);
		void update_revents();
		struct pollfd *get_fds();
		std::list<pollfd> get_lfds();
		
	private:
		struct pollfd _fds[100];
		struct sockaddr_in _addrServer;
		struct msg _msg;

		int parser(std::string cmd, std::list<pollfd>::iterator it, std::list<clients>::iterator it_cli);
		void global_parsing(std::string s, std::list<pollfd>::iterator it, std::list<clients>::iterator it_cli);
		int choose_option(std::string cmd);
		int no_arg(struct msg msg, std::list<pollfd>::iterator it, std::list<clients>::iterator it_cli );
		int one_arg(struct msg msg, std::list<pollfd>::iterator it, std::list<clients>::iterator it_cli );
		int multiple_args(struct msg msg, std::list<pollfd>::iterator it, std::list<clients>::iterator it_cli );
		void build_fds();
		std::string username_with_socket(int socket);
		void display_fds();
		void setup_username( std::string nickname, std::list<clients>::iterator it_cli, int first);
		void setup_password( std::string password, std::list<clients>::iterator it_cli);
		void setup_host( std::string host, std::list<clients>::iterator it_cli );
		void user_left( std::list<pollfd>::iterator it );
		bool channel_open(std::string channel_name, int user);
		void channel_empty(std::string channel_name);
		void commandJOIN( std::list<clients>::iterator it_cli, std::string it );
		void commandNICK( std::list<clients>::iterator it_cli, std::string it );
		void commandPRIVMSG( std::list<clients>::iterator it_cli, std::string message );
		void commandPRIVMSG_user( std::list<clients>::iterator it_cli, std::string it );
		void commandPRIVMSG_channel( std::list<clients>::iterator it_cli, std::string message );
		void commandPART(std::list<clients>::iterator it_cli, std::string it);
		void commandNAME(  std::list<clients>::iterator it_cli );
		void commandLIST(  std::string cmd, std::list<clients>::iterator it_cli );
		void commandQUIT(  std::string cmd , std::list<clients>::iterator it_cli, std::list<pollfd>::iterator it);
		void commandNOTICE( std::list<clients>::iterator it_cli, std::string it );
		void commandMODE( std::list<clients>::iterator it_cli, std::string username, std::string mode, int sender);
		void commandOPER( std::list<clients>::iterator it_cli, std::string username, std::string password);
		void commandKICK(  std::string cmd , std::list<clients>::iterator it_cli );
		void delete_channel(std::list<clients>::iterator it_cli, std::string channel_name);
		bool is_in_the_channel(std::list<std::string> channel, std::string channel_name);
		bool is_in_channel(std::string channel, std::list<std::string> channel_list);
		void create_channel(int user, std::list<clients>::iterator it_cli, std::string channel_name);
		void delete_clrf(std::string temp);
		std::string cut_word_space( std::string to_cut, std::string::iterator it );
		int _clients;
		int _serverSocket;

		std::list<pollfd> _lfds;
		std::list<clients> _user_data;
		std::list<channel> _channel_data;
		std::vector<std::string> cmd;
		std::string _passwd;
		std::string _operpasswd;
	
};

#endif //SERVER_H
