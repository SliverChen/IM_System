#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <string>
#include <windows.h>
#include <winsock2.h>
using std::cin;
using std::getline;
using std::string;

//表示链接ws2_32.lib这个库，将这个库加入到工程文件中
#pragma comment(lib, "ws2_32.lib")

//服务端的类实现
class CTcpServer
{
    SOCKET sServer;
    SOCKET sClient;
    sockaddr_in servaddr;

public:
    CTcpServer();
    ~CTcpServer();
    void bindSock(int port, const char *ipaddr);
    void Listen();
    void connect();
    void Recv();

private:
    void Recv(char *);
    void Send(SOCKET);
};

//************************************************
CTcpServer::CTcpServer()
{
    sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sServer == INVALID_SOCKET)
    {
        perror("socket");
        return;
    }
}

CTcpServer::~CTcpServer()
{
    if (sServer != INVALID_SOCKET)
        closesocket(sServer);
}

void CTcpServer::bindSock(int port, const char *ipaddr)
{
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (ipaddr == nullptr)
        servaddr.sin_addr.s_addr = INADDR_ANY;
    else
        servaddr.sin_addr.s_addr = inet_addr(ipaddr);
    if (bind(sServer, (sockaddr *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
    {
        perror("bind");
        closesocket(sServer);
        return;
    }
}

void CTcpServer::Listen()
{
    if (listen(sServer, 5) == SOCKET_ERROR)
    {
        perror("listen");
        closesocket(sServer);
        return;
    }
}

void CTcpServer::connect()
{
    sockaddr_in clieaddr;
    int clieaddrLen = sizeof(clieaddr);

    sClient = accept(sServer, (SOCKADDR *)&clieaddr, &clieaddrLen);
    if (sClient == SOCKET_ERROR)
    {
        printf("accept error\n");
        return;
    }
    printf("get an connection: %s \r\n", inet_ntoa(clieaddr.sin_addr));
    while (1)
    {
        Recv();
        Send(sClient);
    }
    closesocket(sClient);
    closesocket(sServer);
}

void CTcpServer::Recv()
{
    char recvData[1024];
    memset(recvData, 0, sizeof(recvData));
    Recv(recvData);
    printf("client: %s \n", recvData);
}

void CTcpServer::Send(SOCKET client)
{
    printf("server: ");
    string str;
    getline(cin, str);
    const char *data = str.c_str();
    send(client, data, sizeof(data), 0);
    printf("send message successfully\n");
}

void CTcpServer::Recv(char *buf)
{
    int ret;
    while ((ret = recv(sClient, buf, sizeof(buf), 0)) <= 0)
    {
        Sleep(1);
    }
}

int main()
{
    WSADATA wsaData;
    WORD socketVersion = MAKEWORD(2, 2);
    if (WSAStartup(socketVersion, &wsaData) != 0)
    {
        printf("can't start socket,please check your socket version\n");
        return 0;
    }

    CTcpServer myServer;
    myServer.bindSock(5005, nullptr);
    myServer.Listen();
    myServer.connect();

    WSACleanup();
    return 0;
}