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
	char nom[30];
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
	uint32_t dataLength;
	recv(socketClient , &dataLength,sizeof(uint32_t),0); // Receive the message length
	dataLength = ntohl(dataLength );
	std::vector<char> buffer(dataLength);

	recv(socketClient, &buffer[0], dataLength, 0);

	std::string rcvString;
	rcvString.append(buffer.begin(), buffer.end());

	std::cout << rcvString << std::endl;
	std::cin >> user.nom >> user.age;

	send(socketClient, &user, sizeof(user), 0);
	close(socketClient);

	return 0;
}