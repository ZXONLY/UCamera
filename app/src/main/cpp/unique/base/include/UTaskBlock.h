//
// Created by bytedance on 09/03/2021.
//
#pragma once

#include <UDef.h>

using UTaskBlock = std::function<void()>;

//相对于UETaskQueue，支持任务抢占
UInterface IUTaskPriorityQueue {
        //插入优先任务 & 同步等待执行完
        virtual bool runSyncPriority(UTaskBlock excuteBlock) = 0;

        //插入优先任务 & 不等待执行即返回
        virtual bool runAsyncPriority(UTaskBlock excuteBlock, bool frontQueue = true) = 0;

        //允许暂停异步任务
        virtual void enableBreakAsyncTask(bool enable) = 0;

        //任务队列被插入任务，暂停执行
        virtual bool bExitAsyncTask() = 0;
};
