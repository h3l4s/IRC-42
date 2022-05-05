#include "utils.hpp"

void function(int socketClient)
{
    
    std::string temp = "Welcome to our IRC ! ;) ";
    int len = temp.size();
    const char *msg = temp.c_str();
    send(socketClient, msg, len, 0);
    for (int i = 0; i < 5 ; i++)
    {
        User user;
        recv(socketClient, &user, sizeof(User), 0);
        std::cout << user.msg << std::endl;
        
    }
    return ;
}

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

  
    struct sockaddr_in addrClient;
    socklen_t csize = sizeof(addrClient);
    int socketClient = accept(socketServer, (struct sockaddr *)&addrClient, &csize);
    std::cout << "accept" << std::endl;

    std::cout << "client = " << socketClient << std::endl;
    std::thread client (function, socketClient);
    client.join();

    std::cout << "Done GG !" << std::endl;
    return 0;
}