#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

typedef struct User{
	char name[30];
	int age;
}User;

int main(void){
	int socketClient = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addrClient;
	addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(30000);
	connect(socketClient, (const struct sockaddr *)&addrClient, sizeof(addrClient));
	std::cout << "connected." << std::endl;

	User user;
	std::vector<char> buffer(4096);

	recv(socketClient, &buffer[0], buffer.size(), 0);

	std::string rcvString;
	rcvString.append(buffer.begin(), buffer.end());

	std::cout << rcvString << std::endl;
	std::cin >> user.name >> user.age;

	send(socketClient, &user, sizeof(user), 0);
	int status = 1;
	std::string _quit;
	while (1){
	std::cin >> _quit;
	send(socketClient, _quit.c_str(), _quit.size(), 0);
	recv(socketClient, &status, sizeof(int), 0);
	std::cout << status << std::endl;
	if(status != 1)
		break;
	}
	if(status == -1){
		std::cout << "shutting down client." << std::endl;
		close(socketClient);
		return 0;
	}
	return 1;
}