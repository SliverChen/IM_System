/********************************
 * Topic: 数据包的序列化与反序列化
 * Author: SliverChen
 * Create file date: 2021 / 7 / 16
 * *******************************/

#ifndef SERIAL
#define SERIAL

#pragma once

#include <string>
using std::string;

class Serial
{
public:
    void WriteInt(int i) { append(&i, 4); }
    void WriteString(const string &str)
    {
        WriteInt(str.length());
        append(const_cast<char *>(str.c_str()), str.length());
    }
    const char *getData() { return mbuffer.c_str(); }

private:
    void append(void *buf, size_t n)
    {
        mbuffer.append((const char *)buf, n);
    }
    string mbuffer;
};

class UnSerial
{
public:
    UnSerial(const char *buff)
        : mbuffer(const_cast<char *>(buff)), pos(0) {}

    void clear()
    {
        mbuffer = nullptr;
        pos = 0;
    }
    int ReadInt()
    {
        int i = *(mbuffer + pos);
        pos += 4;
        return i;
    }
    string ReadString()
    {
        int size = ReadInt();
        string s((const char *)mbuffer + pos, size);
        pos += size;
        return s;
    }

private:
    char *mbuffer;
    int pos;
};

#endif