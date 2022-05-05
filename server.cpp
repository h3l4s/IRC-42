#include "utils.hpp"

void function(User user)
{
    int temp2;
    char entry[] = "Please enter your name and age bellow.";
    send(user.socket[0], entry, strlen(entry), 0);
    temp2 = user.socket[0];
    recv(user.socket[0], &user, sizeof(User), 0);
    user.socket[0] = temp2;
    std::cout << "[NEW CLIENT] ID: " << user.socket[0] << " NAME: " << user.name << " AGE: " << user.age << std::endl;
    std::string temp = "you successfully entered the channel.";
    int len = temp.size();
    const char *msg = temp.c_str();
    send(user.socket[0], msg, len, 0);
    while(1)
    {
        Msg _msg;
        recv(user.socket[0], &_msg, sizeof(_msg), 0);
        if(user.socket[0] == 4){
        send(5, &_msg, sizeof(Msg), 0);
        send(6, &_msg, sizeof(Msg), 0);}
        if(user.socket[0] == 5){
        send(4, &_msg, sizeof(Msg), 0);
        send(6, &_msg, sizeof(Msg), 0);}
        if(user.socket[0] == 6){
        send(5, &_msg, sizeof(Msg), 0);
        send(4, &_msg, sizeof(Msg), 0);}
    }
    return ;
}

int main()
{
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;

    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(30006);

    bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
    std::cout << "bind ; " << socketServer << std::endl;

    listen(socketServer, 5);
    std::cout << "listen" << std::endl;

	std::thread client[3];
    User *user = new User[3];
	for ( int i = 0; i < 3; i++ ){
		struct sockaddr_in addrClient;
    	socklen_t csize = sizeof(addrClient);
    	int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
    	std::cout << "accept" << std::endl;
        user[i].socket[0] = socketClient;
    	client[i] = std::thread(function, user[i]);
	}
	for(int i = 0; i < 3; i++ ){
        std::cout << "ID: " << i << " left the channel." << std::endl;
    	client[i].join();
	}

    std::cout << "end" << std::endl;
    return 0;
}