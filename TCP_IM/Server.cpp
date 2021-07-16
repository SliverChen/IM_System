/*********************
 * Topic:一对一聊天系统——Server
 * Author：Sliverchen
 * Date: 2021 / 7 / 9
 * ******************/

#include "Serialize.h"
#include <future>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <winsock2.h>
using std::cin;
using std::future;
using std::getline;
using std::promise;
using std::thread;

#pragma comment(lib, "ws2_32.lib")

//heart-package signal
#define CONNECT_OK 0
#define CONNECT_CLOSE 1

//data-package signal
#define MESSAGE 1
#define HEARTCHECK 0

class Server
{
    SOCKET server;
    SOCKET client;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    struct Package
    {
        int header;
        string mess;
    };

public:
    Server();                  //initialize socket(including wsa)
    ~Server();                 //close socket
    void InitServer(int port); //fulfill the information
    void Listen();             //Listen and wait for client connection
    void sendMess();           //send message to client
    void recvMess();           //receive message from client
    void Close();              //close the connection
    void send_heart();         //send heart data check if the connection is down

private:
    bool isConnected;       //check if is connected
    future<int> fuResult;   //thread connection
    promise<int> myPromise; //set the data structure of the heart-data
private:
    void Recv();
    void Send(Package);
};

Server::Server()
{
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        printf("can't not start up wsa, please check your socket version\n");
        perror("wsaStartup");
        WSACleanup();
        return;
    }
    if ((server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        perror("socket");
        return;
    }
    isConnected = false;
    fuResult = myPromise.get_future();
    printf("init Server socket successfully\n");
}

Server::~Server()
{
    if (server != INVALID_SOCKET)
    {
        closesocket(server);
        isConnected = false;
    }
    WSACleanup();
    printf("Server has disconnected\n");
}

void Server::InitServer(int port)
{
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if (bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        perror("bind");
        return;
    }
}

void Server::Listen()
{
    printf("server listening..\n");
    if (listen(server, 5) == SOCKET_ERROR)
    {
        perror("listen");
        return;
    }
    int clientAddrLen = sizeof(clientAddr);
    if ((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrLen)) == SOCKET_ERROR)
    {
        perror("connect");
        return;
    }
    isConnected = true;
    myPromise.set_value(CONNECT_OK);
    printf("successfully connect with %s \r\n", inet_ntoa(clientAddr.sin_addr));
}

void Server::sendMess()
{
    while (1)
    {
        if (!isConnected || fuResult.get() == CONNECT_CLOSE)
        {
            printf("the connection has shut\n");
            return;
        }
        string data;
        getline(cin, data);

        if (data == "./exit")
        {
            printf("close the connection\n");
            return;
        }
        Package package;
        package.header = MESSAGE;
        package.mess = data;
        Send(package);
    }
}

void Server::Send(Package myPackage)
{
    string data = myPackage.mess;
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

void Server::recvMess()
{
    while (1)
    {
        if (fuResult.get() == CONNECT_CLOSE)
        {
            printf("the connection has shut\n");
            return;
        }
        Recv();
    }
}

void Server::Recv()
{
    char data[1024];
    int ret;
    memset(data, 0, sizeof(data));
    while ((ret = recv(client, data, sizeof(data), 0)) <= 0)
    {
        if (server == INVALID_SOCKET)
        {
            printf("stop receiving from client\n");
            return;
        }
        Sleep(1);
    }
    data[ret] = 0x00;
    printf("Friend: %s\n", data);
}

void Server::Close()
{
    if (server != INVALID_SOCKET)
        closesocket(server);
    isConnected = false;
    myPromise.set_value(1);
}

void Server::send_heart()
{
    const char data[1024] = "check\0";
    while (send(client, data, strlen(data), 0) != SOCKET_ERROR)
    {
        //need to check errno
        Sleep(1000);
    }
    isConnected = false;
    myPromise.set_value(CONNECT_CLOSE);
}

int main()
{
    Server myServer;
    myServer.InitServer(5005);
    myServer.Listen();
    thread t1(&Server::recvMess, &myServer);
    t1.detach();
    thread t2(&Server::send_heart, &myServer);
    t2.detach();
    myServer.sendMess();
    myServer.Close();
    return 0;
}