#include "utils.hpp"

void function(int socketClient){
    Msg _msg;
    while(1){
        recv(socketClient, &_msg, sizeof(Msg), 0);
        std::cout << "[Incoming message]: " << _msg.msg << std::endl;
    }
}

int main()
{
    int socketClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addrClient;

    addrClient.sin_addr.s_addr = inet_addr("127.0.0.1");
    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(30006);

    connect(socketClient, (const struct sockaddr *)&addrClient, sizeof(addrClient));
    std::cout << "Connecte" << std::endl;



    char msg[500];
    recv(socketClient, &msg, 500, 0);
    User user;
    std::cout << msg << std::endl;
    std::cin >> user.name >> user.age;
    send(socketClient, &user, sizeof(User), 0);
    std::string temp;
    Msg _msg;
    char msg2[15];
    recv(socketClient, &msg2, 38, 0);
    std::cout << msg2 << std::endl;
    std::thread client(function, socketClient);
    while (1)
    {
        getline(std::cin, temp);
        _msg.len = temp.size();
        strcpy(_msg.msg, temp.c_str());
        send(socketClient, &_msg, sizeof(_msg), 0);
        temp.replace(0, temp.size(), "");
    }
    client.join();
    std::cout << "end" << std::endl;
    return 0;
}