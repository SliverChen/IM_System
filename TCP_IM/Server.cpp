/*********************
 * Topic:一对一聊天系统——Server
 * Author：Sliverchen
 * Date: 2021 / 7 / 9
 * ******************/

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

class Server;
SOCKET server;
void recvMess(const Server &);

class Server
{
    SOCKET client;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    // friend void recvMess(const Server &); //receive message from client
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
    void Recv();
    void Send(string data);
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
    printf("init Server socket successfully\n");
}

Server::~Server()
{
    if (server != INVALID_SOCKET)
        closesocket(server);
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
    printf("successfully connect with %s \r\n", inet_ntoa(clientAddr.sin_addr));
}

void Server::sendMess()
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

void Server::Send(string data)
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

void Server::recvMess()
{
    while (1)
    {
        if (server == INVALID_SOCKET)
            break;
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
}

int main()
{
    Server myServer;
    myServer.InitServer(5005);
    myServer.Listen();
    thread t(&Server::recvMess, &myServer);
    myServer.sendMess();
    myServer.Close();
    t.join();
    return 0;
}