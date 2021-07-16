/*******************
 * Topic: 一对一聊天系统——Client
 * Author:Sliverchen
 * Date: 2021 / 7 / 9
 ******************/

#include "Serialize.h"
#include <iostream>
#include <stdio.h>
#include <thread>
#include <winsock2.h>
using std::cin;
using std::getline;
using std::thread;

#pragma comment(lib, "ws2_32.lib")

class Client
{
    SOCKET client;
    sockaddr_in serverAddr;

public:
    Client();                //Initialize the client socket and startup the wsa
    ~Client();               //close the socket if is valid and clean the wsa
    void InitClient(string); //initialize the sockaddr
    void Connect(int port);  //connect to the indirected port
    void sendMess();         //send message to server
    void recvMess();         //receive message from server
    void Close();            //close the socket
    void send_heart();       //send heart data to check if the connection is down
    // friend void recvMess(const Client &); //receive message from server

private:
    void Send(string data);
    void Recv();
};

Client::Client()
{
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        printf("can't not start wsa,please check socket version\n");
        perror("wsaStartup");
        WSACleanup();
        return;
    }
    if ((client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        perror("socket");
        return;
    }
    printf("init Client socket successfully\n");
}

Client::~Client()
{
    if (client != INVALID_SOCKET)
        closesocket(client);
    WSACleanup();
    printf("client has disconnected\n");
}

void Client::InitClient(string ip)
{
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
}

void Client::Connect(int port)
{
    serverAddr.sin_port = htons(port);
    if (connect(client, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        perror("connect");
        return;
    }
    printf("successfully connect with server\n");
}

void Client::recvMess()
{
    while (1)
    {
        if (client == INVALID_SOCKET)
            break;
        Recv();
    }
}

void Client::Recv()
{
    char data[1024];
    int ret;
    memset(data, 0, sizeof(data));
    while ((ret = recv(client, data, sizeof(data), 0)) <= 0)
    {
        if (client == INVALID_SOCKET)
        {
            printf("stop receiving from server\n");
            return;
        }
        Sleep(1);
    }
    data[ret] = 0x00;
    printf("Friend: %s\n", data);
}

void Client::sendMess()
{
    while (1)
    {
        string data;
        getline(cin, data);
        if (data == "./exit")
        {
            printf("close the connection\n");
            return;
        }
        Send(data);
    }
}

void Client::Send(string data)
{
    const char *mess = data.c_str();
    if (send(client, mess, strlen(mess), 0) == SOCKET_ERROR)
    {
        if (client == INVALID_SOCKET)
            printf("the connection has closed\n");
        else
            printf("can't not send the message,please check the sockAddr information\n");
        return;
    }
}

void Client::Close()
{
    if (client != INVALID_SOCKET)
    {
        closesocket(client);
    }
}

void Client::send_heart()
{
}

int main()
{
    Client myclient;
    myclient.InitClient("127.0.0.1");
    myclient.Connect(5005);

    thread t(&Client::recvMess, &myclient);
    myclient.sendMess();
    myclient.Close();
    t.join();
    return 0;
}