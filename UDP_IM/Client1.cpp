/*********************************
 * Topic: 基于UDP的IM系统——Client1
 * Author:SliverChen
 * Date : 2021 / 7 / 9
 * *****************************/

#include <iostream>
#include <stdio.h>
#include <string>
#include <winsock2.h>
using std::cin;
using std::getline;
using std::string;

#pragma comment(lib, "ws2_32.lib")

class Client1
{
    SOCKET client;
    sockaddr_in clientAddr;

public:
    Client1();
    ~Client1();
    void Init(int);
    void connect();
    void sendMess();
    void recvMess();

private:
    void Send(string data);
    void Recv();
};

Client1::Client1()
{
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsaData) != 0)
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

Client1::~Client1()
{
    if (client != INVALID_SOCKET)
    {
        closesocket(client);
    }
    WSACleanup();
}

void Client1::Init(int port)
{
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = INADDR_ANY;
    clientAddr.sin_port = htons(port);
    if (bind(client, (SOCKADDR *)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
    {
        perror("bind");
        return;
    }
}

void Client1::sendMess()
{
    string data;
    while (1)
    {
        getline(cin, data);
        Send(data);
    }
}

void Client1::Send(string data)
{

    const char *mess = data.c_str();
    if (sendto(client, mess, strlen(mess), 0, (SOCKADDR *)&))
}