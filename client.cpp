#include "server.hpp"




int main()
{
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrClient;

    addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(30002);

    connect(socketClient, (const struct sockaddr *)&addrClient, sizeof(addrClient));
    std::cout << "Connecte" << std::endl;



    char msg[500];
    recv(socketClient, &msg, 500, 0);
    std::cout << msg << std::endl;
    std::string temp;
    User user;
    while(1){
        getline(std::cin, temp);
        user.len = temp.size();
        strcpy(user.msg, temp.c_str());
        send(socketClient, &user, sizeof(User), 0);
        temp.replace(0, temp.size(), "");
    }

    return 0;
}