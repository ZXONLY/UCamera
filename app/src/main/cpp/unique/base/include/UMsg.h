//
// Created by bytedance on 10/03/2021.
//
#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H

#include <chrono>
#include <functional>
#include <string>

class UMsg{
public:
    int m_what;
    int m_arg1;
    int m_arg2;
    std::string m_arg3;
    double m_arg4;
    typedef std::function<void()> Function;
    Function task;

    std::chrono::system_clock::time_point when;

public:
    UMsg();
    UMsg(int what);
    UMsg(int what, int arg1);
    UMsg(int what, int arg1, int arg2);
    UMsg(int what, int arg1, int arg2,long uptimeMillis);
    UMsg(int what, int arg1, int arg2,std::string arg3, double arg4,long uptimeMillis);
    virtual ~UMsg();

    UMsg& operator=(const UMsg& msg);

    void setWhen(long uptimeMillis);

    void setFunction(std::function<void()> &&f);

    bool operator > (const UMsg& msg) const {
        return (this->when > msg.when);
    }

    bool operator < (const UMsg& msg) const {
        return (this->when < msg.when);
    }

    bool operator==(const UMsg& msg) const {
        return (this->m_what == msg.m_what) && (this->task != nullptr) && (msg.task != nullptr);
    }

    bool operator==(int what) const {
        return (this->m_what == what);
    }

private:

};

#endif


