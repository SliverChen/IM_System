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
void recvMess(const Server &);

class Server
{
    SOCKET server;
    SOCKET client;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;
    friend void recvMess(const Server &); //receive message from client
public:
    Server();                  //initialize socket(including wsa)
    ~Server();                 //close socket
    void InitServer(int port); //fulfill the information
    void Listen();             //Listen and wait for client connection
    void sendMess();           //send message to client
    void close();              //close the connection

private:
    void Recv() const;
    void Send(string data);
};

Server::Server()
{
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        printf("can't not start up wsa, please check your socket version\n");
        return;
    }
    if ((server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        perror("socket");
        return;
    }
}

Server::~Server()
{
    if (server != INVALID_SOCKET)
        closesocket(server);
    WSACleanup();
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
    string data;
    printf("you: ");
    getline(cin, data);
    Send(data);
}

void Server::Send(string data)
{
    const char *mess = data.c_str();
    if (send(client, mess, sizeof(mess), 0) == SOCKET_ERROR)
    {
        printf("can't not send the message,please check the sockAddr information\n");
        return;
    }
}

void recvMess(const Server &myServer)
{
    while (1)
    {
        if (myServer.server == INVALID_SOCKET)
            break;
        myServer.Recv();
    }
}

void Server::Recv() const
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

void Server::close()
{
    if (server != INVALID_SOCKET)
        closesocket(server);
}

int main()
{
    Server myServer;
    myServer.InitServer(5005);
    myServer.Listen();
    thread t(recvMess, myServer);
    myServer.sendMess();
    t.join();
    myServer.close();
    return 0;
}