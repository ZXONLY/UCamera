//
// Created by bytedance on 10/03/2021.
//

#include "include/UMsg.h"
#include <chrono>
#include <utility>


UMsg::UMsg():UMsg(0, 0, 0, 0){}

UMsg::UMsg(int what):UMsg(what, 0, 0, 0){}

UMsg::UMsg(int what, int arg1):UMsg(what, arg1, 0, 0){}

UMsg::UMsg(int what, int arg1, int arg2):UMsg(what, arg1, arg2, 0){}

UMsg::UMsg(int what, int arg1, int arg2,std::string arg3, double arg4,long uptimeMillis):m_what(what),m_arg1(arg1),m_arg2(arg2),m_arg3(std::move(arg3)),m_arg4(arg4){
    when = std::chrono::system_clock::now() + std::chrono::milliseconds(uptimeMillis);
    task = nullptr;
}

UMsg::UMsg(int what, int arg1, int arg2,long uptimeMillis):UMsg(what,arg1,arg2,"",0.0,0){

}



void UMsg::setWhen(long uptimeMillis){
    when = std::chrono::system_clock::now() + std::chrono::milliseconds(uptimeMillis);
}

void UMsg::setFunction(std::function<void()> &&f){
    this->task = f;
}
UMsg::~UMsg(){

}

UMsg& UMsg::operator = (const UMsg& msg) {
    this->m_arg1 = msg.m_arg1;
    this->m_arg2 = msg.m_arg2;
    this->m_arg3 = msg.m_arg3;
    this->m_arg4 = msg.m_arg4;
    this->m_what = msg.m_what;
    this->when = msg.when;
    this->task = msg.task;

    return *this;
}