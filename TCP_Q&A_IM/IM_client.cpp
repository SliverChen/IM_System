#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <winsock2.h>
using std::cin;
using std::getline;
using std::string;

//表示链接ws2_32.lib这个库，将这个库加入到工程文件中
#pragma comment(lib, "ws2_32.lib")

class CTcpClient
{
    int clientfd;
    const char *ipaddr;
    sockaddr_in servaddr;

public:
    CTcpClient(const char *_ipaddr);
    ~CTcpClient();
    void Connect(int port);
    void Send();
    void Recv(char *);
};

CTcpClient::CTcpClient(const char *_ipaddr)
{
    clientfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientfd == INVALID_SOCKET)
    {
        perror("socket");
        return;
    }
    ipaddr = _ipaddr;
}

CTcpClient::~CTcpClient()
{
    if (clientfd != INVALID_SOCKET)
        closesocket(clientfd);
}

void CTcpClient::Connect(int port)
{
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ipaddr);
    servaddr.sin_port = htons(port);
    if (connect(clientfd, (SOCKADDR *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
    {
        perror("connect");
        closesocket(clientfd);
        return;
    }
    printf("connect to the server,which port is %d\n", port);

    while (1)
    {
        Send();
        printf("send message successfully\n");
        char recData[1024];
        memset(recData, 0, sizeof(recData));
        Recv(recData);
        printf("server: %s \n", recData);
    }
    closesocket(clientfd);
}

void CTcpClient::Send()
{
    printf("client: ");
    string data;
    getline(cin, data);
    const char *sendData = data.c_str();
    send(clientfd, sendData, strlen(sendData), 0);
}

void CTcpClient::Recv(char *buf)
{
    int ret;
    while ((ret = recv(clientfd, buf, sizeof(buf), 0)) <= 0)
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
        printf("can't start socket, please check your socket version\n");
        return 0;
    }
    CTcpClient myClient("127.0.0.1");
    myClient.Connect(5005);

    WSACleanup();
    return 0;
}