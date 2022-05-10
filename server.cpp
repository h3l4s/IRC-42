#include "utils.hpp"


int main()
{
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrServer;

    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(30003);

    bind(socketServer, (const struct sockaddr *)&addrServer, sizeof(addrServer));
    std::cout << "bind ; " << socketServer << std::endl;

    listen(socketServer, 5);
    std::cout << "listen" << std::endl;
    
    //int *socketClient = new int;
    //for(int i = 0; i < 2; i++)
    //{
    //    struct sockaddr_in addrClient;
    //    socklen_t csize = sizeof(addrClient);
    //    socketClient[i] = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
    //    std::cout << "accept" << std::endl;
    //}
    User user;
    std::string temp = "Welcome to our IRC ! ;) ";
    int len = temp.size();
    const char *msg = temp.c_str();
    //send(socketClient[0], msg, len, 0);
    //send(socketClient[1], msg, len, 0);
    struct pollfd fds[100];
    //for (int i = 0; i < 2 ; i++){
    //    fds[i].fd = socketClient[i];
    //    fds[i].events = POLLIN;
    //}
	int i = 1;
	fds[0].fd = socketServer;
	fds[0].events = POLLIN;
    while(1)
    {
		int e = poll(fds, i, 10);
		if(fds[0].revents & POLLIN){
			int socketClient;
			struct sockaddr_in addrClient;
    		socklen_t csize = sizeof(addrClient);
    		socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
    		std::cout << "accept" << std::endl;
			fds[i].fd = socketClient;
			fds[i].events = POLLIN;
			send(fds[i].fd, msg, len, 0);
			i++;
		}
		for(int x = 1; x < i; x++){
        if(fds[x].revents & POLLIN){
            if(recv(fds[x].fd, &user, sizeof(User), 0) == 0){
				std::cout << "USER: " << fds[x].fd << " disconnected." << std::endl;
				close(fds[x].fd);
			}
			else 
            	std::cout << user.msg << std::endl;
        }
		//if(fds[x].revents & POLLHUP){
        //    std::cout << "USER: " << fds[x].fd << " disconnected." << std::endl;
		//	sleep(3);
        //}
		}
	}
    return 0;
}