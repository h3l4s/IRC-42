#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <vector>

typedef struct User{
	char name[30];
	int age;
}User;

void *function(void *arg){
	int socket = *(int*)arg;
	//const char msg[] = "what's your name and how old are you";
	const std::string msg = "what's your name and how old are you";
	User user;
	send(socket, msg.c_str(), msg.size(), 0);
	recv(socket, &user, sizeof(user), 0);
	std::cout << "client's name: " << user.name << " | client's age: " << user.age << " years old." << std::endl;
	
	std::string rcvMsg = "";
	std::vector<char> buffer(4096);
	int status = 1;

	while (1){
		if(rcvMsg != "")
			rcvMsg.clear();
		recv(socket, &buffer[0], buffer.size(), 0);
		rcvMsg.append(buffer.begin(), buffer.end());
		std::cout << rcvMsg << std::endl;
		if(buffer[0] == '/' && buffer[1] == 'q')
			break;
		send(socket, &status, sizeof(int), 0);
	}
	std::cout << "client: " << user.name << " id: " << socket << " left." << std::endl;
	status = -1;
	send(socket, &status, sizeof(int), 0);
	close(socket);
	delete((int*)arg);
	pthread_exit(NULL);
}

int main(void){
	int socketServer = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addrServer;
	addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(30000);

	bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
	std::cout << "bind: " << socketServer << std::endl;

	listen(socketServer, 5);
	std::cout << "listen" << std::endl;

	pthread_t threads[3];

	for(int i = 0; i < 3; i++){
		struct sockaddr_in addrClient;
		socklen_t csize = sizeof(addrClient);
		int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
		std::cout << "accept" << std::endl;

		std::cout << "client: " << socketClient << std::endl;
		
		int *arg = new int();
		*arg = socketClient;
		pthread_create(&threads[i], NULL, function, arg);
	}

	for(int i = 0; i < 3; i++){
		pthread_join(threads[i], NULL);
	}

	//send(socketClient, &user, sizeof(user), 0);

	close(socketServer);
	std::cout << "close" << std::endl;
	return 0;
}