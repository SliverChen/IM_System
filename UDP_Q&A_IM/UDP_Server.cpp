#include <iostream>
#include <stdio.h>
#include <string>
#include <winsock2.h>
using std::cin;
using std::getline;
using std::string;

#pragma comment(lib, "ws2_32.lib")

class UdpServer
{
public:
    UdpServer();
    ~UdpServer();
    bool isConnected() { return m_isConnected; }
    void Init(int port);
    void SendMess(string mMess);
    void RecvMess();
    void close()
    {
        if (serverfd != INVALID_SOCKET)
            closesocket(serverfd);
        m_isConnected = false;
    }

private:
    bool m_isConnected;
    SOCKET serverfd;
    sockaddr_in serveraddr;
    sockaddr_in clientaddr;
};

UdpServer::UdpServer()
{
    serverfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverfd == INVALID_SOCKET)
    {
        perror("socket");
        return;
    }
    m_isConnected = false;
}

UdpServer::~UdpServer()
{
    if (serverfd != INVALID_SOCKET)
        close();
    m_isConnected = false;
}

void UdpServer::Init(int port)
{
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverfd, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
    {
        perror("bind");
        close();
        return;
    }
}

void UdpServer::SendMess(string mMess)
{
    const char *data = mMess.c_str();
    if (sendto(serverfd, data, strlen(data), 0, (SOCKADDR *)&clientaddr, sizeof(clientaddr)) == SOCKET_ERROR)
    {
        printf("fail to send\n");
        return;
    }
}

void UdpServer::RecvMess()
{
    int clientAddrLen = sizeof(clientaddr);

    char recvData[1024];
    int ret;
    while ((ret = recvfrom(serverfd, recvData, sizeof(recvData), 0, (SOCKADDR *)&clientaddr, &clientAddrLen)) <= 0)
    {
        Sleep(1);
    }
    printf("client: %s\n", recvData);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        printf("can't start up socket, please check your socket version\n");
        return 0;
    }

    UdpServer myServer;
    myServer.Init(5005);

    while (1)
    {
        myServer.RecvMess();
        printf("server:");
        string message;
        getline(cin, message);
        myServer.SendMess(message);
    }
    myServer.close();
    WSACleanup();
    return 0;
}