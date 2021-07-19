/*********************
 * Topic:一对一聊天系统——Server
 * Author：Sliverchen
 * Date: 2021 / 7 / 9
 * ******************/

#include "predefined.h"
#include <future>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <winsock2.h>
using std::cin;
using std::cout;
using std::future;
using std::getline;
using std::mutex;
using std::promise;
using std::thread;

class Server
{
    SOCKET server;
    SOCKET client;
    sockaddr_in serverAddr;
    sockaddr_in clientAddr;

public:
    Server();                  //initialize socket(including wsa)
    ~Server();                 //close socket
    void InitServer(int port); //fulfill the information
    void Listen();             //Listen and wait for client connection
    void sendMess();           //send message to client
    void recvMess();           //receive message from client
    void Close();              //close the connection
    void send_heart();         //send heart data check if the connection is down
    bool isClosed(SOCKET);     //check client socket if is down

private:
    bool isConnected;       //check if is connected
    future<int> fuResult;   //thread connection
    promise<int> myPromise; //set the data structure of the heart-data
    mutex mtx;              //thread protection

private:
    void Recv();
    int Send(string, int);
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
    Close();
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

        if (data == "./exit\0")
        {
            printf("close the connection\n");
            return;
        }
        Send(data, CLIENT_MESS);
    }
}

int Server::Send(string data, int signal)
{
    /*  Serialize  */
    Serial mySerial;
    mySerial.WriteInt(signal);
    mySerial.WriteString(data);

    const char *mess = mySerial.getData();
    int ret = send(client, mess, strlen(mess), 0);
    if (ret == SOCKET_ERROR)
    {
        if (client == INVALID_SOCKET)
            printf("the connection has closed\n");
        else
            printf("can't send the message,please check the sockAddr information\n");
        return -1;
    }
    printf("send successfully, which ret is %d\n", ret);
    return 1;
}

void Server::recvMess()
{
    while (1)
    {
        Recv();
    }
}

void Server::Recv()
{
    char data[DATASIZE];
    int ret;

    while ((ret = recv(client, data, DATASIZE, 0)) <= 0)
    {
        Sleep(1);
    }

    printf("receive the data where ret is %d\n", ret);

    /*  Unserialize the data  */
    UnSerial myunSerial(data);
    int head = myunSerial.ReadInt();
    string str = myunSerial.ReadString();

    if (head == HEART_MESS)
    {
        cout << "get the heart mess: " << str << "\n";
        Send(HEART_DATA, HEART_MESS);
    }
    else
    {
        printf("Friend:%s\n", str.c_str());
    }
    myunSerial.clear();
}

void Server::Close()
{
    if (server != INVALID_SOCKET)
        closesocket(server);
}

bool Server::isClosed(SOCKET client)
{
    char buff[32];
    int recvBytes = recv(client, buff, sizeof(buff), MSG_PEEK);
    int sockErr = errno;

    if (recvBytes > 0)
        return false;

    if (recvBytes == -1 && sockErr == EWOULDBLOCK)
        return false;

    return true;
}

int main()
{
    Server myServer;
    myServer.InitServer(5005);
    myServer.Listen();
    thread t(&Server::recvMess, &myServer);
    t.detach();
    myServer.sendMess();
    myServer.Close();
    return 0;
}