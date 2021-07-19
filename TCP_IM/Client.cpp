/*******************
 * Topic: 一对一聊天系统——Client
 * Author:Sliverchen
 * Date: 2021 / 7 / 9
 ******************/

#include "predefined.h"
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <winsock2.h>
using std::cin;
using std::cout;
using std::getline;
using std::mutex;
using std::thread;

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

private:
    mutex mtx; //thread protection

private:
    int Send(string, int);
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
    Close();
    WSACleanup();
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
        Recv();
    }
}

void Client::Recv()
{
    char data[DATASIZE];
    int ret;
    while ((ret = recv(client, data, DATASIZE, 0)) <= 0)
    {
        Sleep(1);
    }
    printf("receive the data where ret is %d\n", ret);

    /* UnSerial the data */
    UnSerial myunSerial(data);
    int head = myunSerial.ReadInt();
    string str = myunSerial.ReadString();

    if (head == HEART_MESS)
    {
        cout << "connection is OK\n";
    }
    else
    {
        printf("Friend:%s\n", str.c_str());
    }
    myunSerial.clear();
}

void Client::sendMess()
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

int Client::Send(string data, int signal)
{
    //Serialize
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

    printf("doing send successfully, which ret is %d\n", ret);

    if (signal == CLIENT_MESS)
        printf("the main thread: send successfully, which ret is %d \n", ret);

    return 1;
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
    int time = 0;
    while (1)
    {
        Sleep(5000);
        int ret = Send(HEART_DATA, HEART_MESS);
        if (ret == -1 && time < 5)
        {
            printf("send heart data fail, try to resend the data..\n");
            time++;
            Sleep(5000);
        }
        else if (time >= 5)
        {
            printf("resend fail,closing the connection..\n");
            Close();
            return;
        }
        else
        {
            printf("heart thread: send successfully, which ret is %d \n", ret);
        }
    }
}

int main()
{
    Client myclient;
    myclient.InitClient("127.0.0.1");
    myclient.Connect(5005);
    thread t1(&Client::send_heart, &myclient);
    t1.detach();
    thread t2(&Client::recvMess, &myclient);
    t2.detach();
    myclient.sendMess();
    myclient.Close();
    return 0;
}