#include <iostream>
#include <stdio.h>
#include <string>
#include <winsock2.h>
using std::cin;
using std::getline;
using std::string;
#pragma comment(lib, "ws2_32.lib")

class UdpClient
{
public:
    UdpClient();
    ~UdpClient();
    void Init(string address, int port);
    bool IsConnected() { return m_IsConnected; }
    void SendMess(string sMessage);
    void RecvMess();
    void close()
    {
        if (clientfd != INVALID_SOCKET)
            closesocket(clientfd);
        m_IsConnected = false;
    }

private:
    bool m_IsConnected;
    int clientAddrLen;
    sockaddr_in clientAddr;
    SOCKET clientfd;
};

UdpClient::UdpClient()
{
    m_IsConnected = false;
    clientfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientfd == INVALID_SOCKET)
    {
        perror("socket");
        return;
    }
}

UdpClient::~UdpClient()
{
    if (m_IsConnected || clientfd != INVALID_SOCKET)
        closesocket(clientfd);
}

void UdpClient::Init(string address, int port)
{
    const char *ip = address.c_str();
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = inet_addr(ip);
    clientAddr.sin_port = htons(port);
    clientAddrLen = sizeof(clientAddr);
}

void UdpClient::SendMess(string sMessage)
{
    const char *sendData = sMessage.c_str();
    if (sendto(clientfd, sendData, strlen(sendData), 0, (SOCKADDR *)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
    {
        printf("failed to send\n");
        return;
    }
    m_IsConnected = true;
}

void UdpClient::RecvMess()
{
    int ret;
    char recvData[1024];
    while ((ret = recvfrom(clientfd, recvData, sizeof(recvData), 0, (SOCKADDR *)&clientAddr, &clientAddrLen)) <= 0)
    {
        Sleep(1);
    }
    printf("server: %s\n", recvData);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    WORD socketVersion = MAKEWORD(2, 2);
    if (WSAStartup(socketVersion, &wsaData) != 0)
    {
        printf("can't start wsa, please check your socketVersion\n");
        return 0;
    }
    UdpClient myClient;
    myClient.Init("127.0.0.1", 5005);
    while (1)
    {
        printf("client:");
        string myWords;
        getline(cin, myWords);
        myClient.SendMess(myWords);
        myClient.RecvMess();
    }
    myClient.close();
    WSACleanup();
    return 0;
}
