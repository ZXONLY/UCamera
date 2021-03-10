//
// Created by bytedance on 09/03/2021.
//
#include "include/IUTaskQueue.h"
#include <sstream>

static void *threadQueueFunc(void *arg) {
    auto *pThis = static_cast<IUTaskQueue *>(arg);
    return pThis->queueThreadFunc();
}

void *IUTaskQueue::queueThreadFunc() {
    UTaskBlock block = nullptr;

    int lowPriorityTaskCount = 0;
    int highPriorityTaskCount  = 0;

    if (mThread ){
        std::ostringstream nameStream;
        nameStream << m_sName << "-" << (void*)this;
        std::string newName = nameStream.str();
        //mThread->set
    }
}