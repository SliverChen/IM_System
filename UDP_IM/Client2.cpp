/***********************************
 * Topic: 基于UDP的IM系统——Client2
 * Author:Sliverchen
 * Date: 2021 / 7 / 9
 * *********************************/

#include <iostream>
#include <stdio.h>
#include <string>
#include <winsock2.h>
using std::cin;
using std::getline;
using std::string;

#pragma comment(lib, "ws2_32.lib")

class Client2
{
    SOCKET client;
    sockaddr_in clientAddr;

public:
    Client2();
    ~Client2();
    void Init();
    void connect();
    void sendMess();
    void recvMess();

private:
    void Send(string data);
    void Recv();
};

Client2::Client2()
{
    WSADATA wsaData;
    WORD socketVersion = MAKEWORD(2, 2);
    if (WSAStartup(socketVersion, &wsaData) != 0)
    {
        printf("please check the socket version\n");
        return;
    }
    if (client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) == INVALID_SOCKET)
    {
        perror("socket");
        return;
    }
}

Client2::~Client2()
{
    if (client != INVALID_SOCKET)
    {
        closesocket(client);
    }
    WSACleanup();
}

void Client2::Init()
{
    clientAddr.sin_family = AF_INET;
}