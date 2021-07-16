#ifndef PREDEFINED_H
#define PREDEFINED_H

#pragma once
#pragma comment(lib, "ws2_32.lib")

//serialize & unserialize
#include "Serialize.h"

//heart-package signal
#define CONNECT_OK 0
#define CONNECT_BAD 1
#define CONNECT_FAIL 2

//data-package singal
#define CLIENT_MESS 0
#define HEART_MESS 1

//the data structure of Message
struct Package
{
    int header;
    string mess;
    Package(int _h, string _m) : header(_h), mess(_m) {}
};

#endif