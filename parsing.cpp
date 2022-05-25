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

typedef struct msg{
	std::string cmd;
	std::string prefix;
	std::string args;
	std::string result[3] = { prefix, cmd, args };
} msg;

void no_arg(struct msg msg){
	std::cout << "appear in no_arg" << std::endl;
	//need to execute /QUIT here or redirect
	return ;
}

void one_arg(struct msg msg){
	std::cout << "appear in one_arg" << std::endl;
	//need to execute /PASS & /NICK here or redirect
	return ;
}

void multiple_args(struct msg msg){
	std::cout << "appear in multiple_args" << std::endl;
	//need to parse the multiple args properly
	return ;
}

int choose_option(std::string cmd){
	std::string options[13] = { "QUIT", "PASS", "NICK",
	"USER", "OPER", "JOIN", "PART", "MODE", "TOPIC",
	"NAMES", "LIST", "INVITE", "KICK" };
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

void global_parsing(std::string s){
	std::string delimiter = " ";
	size_t pos = 0;
	int i;
	msg msg;

	s[0] == ':' ? i = 0 : i = 1; // checking if a prefix is present
	if(i == 1)
		msg.result[0] = "";
	while ((pos = s.find(delimiter)) != std::string::npos && i != 2) { // splitting using spaces
		msg.result[i] = s.substr(0, pos);
    	s.erase(0, pos + delimiter.length());
		i++;
	}
	msg.result[i] = s;
	i = choose_option(msg.result[1]);
	if(i == -1){
		std::cout << "error: command not found." << std::endl;
		return ;
	}
	void (*options_function[])(struct msg) = { no_arg, one_arg, multiple_args };
	options_function[i](msg);
	return ;
}

int parser(std::string cmd, std::list<pollfd>::iterator it){
	std::string input;

	input.assign(cmd);
	std::cout << "'" << input << "'" << std::endl;
	global_parsing(input);
	return 0;
}