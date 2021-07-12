/*******************
 * Topic: 一对一聊天系统——Client
 * Author:Sliverchen
 * Date: 2021 / 7 / 9
 ******************/

#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>
#include <winsock2.h>
using std::cin;
using std::getline;
using std::string;
using std::thread;

#pragma comment(lib, "ws2_32.lib")

class Client
{
    SOCKET client;
    sockaddr_in clientAddr;

public:
    Client();                             //Initialize the client socket and startup the wsa
    ~Client();                            //close the socket if is valid and clean the wsa
    void InitClient(string);              //initialize the sockaddr
    void Connect(int port);               //connect to the indirected port
    void sendMess();                      //send message to server
    friend void recvMess(const Client &); //receive message from server
    void close();                         //close the socket

private:
    void Send(string data);
    void Recv() const;
};

Client::Client()
{
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        printf("can't not start wsa,please check socket version\n");
        return;
    }
    if ((client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        perror("socket");
        return;
    }
}

Client::~Client()
{
    if (client != INVALID_SOCKET)
        closesocket(client);
    WSACleanup();
}

void Client::InitClient(string ip)
{
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = inet_addr(ip.c_str());
}

void Client::Connect(int port)
{
    clientAddr.sin_port = htons(port);
    if (connect(client, (SOCKADDR *)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
    {
        perror("connect");
        return;
    }
    printf("successfully connect with server\n");
}

void recvMess(const Client &myClient)
{
    while (1)
    {
        if (myClient.client == INVALID_SOCKET)
            break;
        myClient.Recv();
    }
}

void Client::Recv() const
{
    char data[1024];
    int ret;
    while ((ret = recv(client, data, sizeof(data), 0)) <= 0)
    {
        Sleep(1);
    }
    data[ret] = 0x00;
    printf("Friend: %s\n", data);
}

void Client::sendMess()
{
    string data;
    printf("you: ");
    getline(cin, data);
    Send(data);
}

void Client::Send(string data)
{
    const char *mess = data.c_str();
    if (send(client, mess, sizeof(mess), 0) == SOCKET_ERROR)
    {
        printf("can't not send the message,please check the sockAddr information\n");
        return;
    }
}

void Client::close()
{
    if (client == INVALID_SOCKET)
    {
        closesocket(client);
    }
}

int main()
{
    Client myclient;
    myclient.InitClient("127.0.0.1");
    myclient.Connect(5005);

    thread t(recvMess, myclient);
    myclient.sendMess();
    t.join();
    myclient.close();
    return 0;
}