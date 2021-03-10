//
// Created by bytedance on 09/03/2021.
//
#pragma once

#include <list>
#include <functional>
#include <UThreadId.h>
#include <UTask.h>
#include <string>
#include <mutex>
#include "IUThreadPool.h"
#include <UTaskBlock.h>


using UPipelineTaskBlock = std::function<void(core::UPipeline *)>;

class IUTaskQueue : public IUTaskPriorityQueue{
public:

    virtual bool isVaild() = 0;
    virtual ~IUTaskQueue() {};
    virtual pthread_id_t getTid();

public:
    virtual void *queueThreadFunc();

    //异步执行任务，附带Pipeline参数，对齐Android现有代码
    virtual bool runAsync(core::UPipeline *pipeline, UPipelineTaskBlock exe) ;

    //异步执行任务，附带Pipeline参数，对齐Android现有代码
    virtual bool runAsyncunsafe(core::UPipeline *pipeline, UPipelineTaskBlock exe);

    //同步执行任务
    //    virtual bool runSync(core::TEVideoPipeline * pipeline, TEPipelineTaskBlock exe) override;

    // process the task queue
    virtual void processTaskQueue(std::function<void(std::list<core::TETask> &)>);

public:
    //插入优先任务 & 同步等待执行完
    virtual bool runSyncPriority(UTaskBlock excuteBlock) override;

    //插入优先任务 & 不等待执行即返回
    virtual bool runAsyncPriority(UTaskBlock excuteBlock, bool frontQueue = true) override;

    //允许暂停异步任务
    virtual void enableBreakAsyncTask(bool enable) override { mbExitAsyncTask = enable; }

    //任务队列被插入任务，暂停执行
    virtual bool bExitAsyncTask() override { return mbExitAsyncTask; }


public:
    //暂停任务执行
    virtual void blockQueue() = 0;
    //恢复任务执行
    virtual void activeQueue() = 0;
    //结束任务执行
    virtual void stopQueue() = 0;

    virtual size_t getTaskCount() = 0;

    //队列满时，触发回调处理
    //virtual void setOnAsyncEnqueueBlock(TEAsyncOnPushBlock onFullBlock) = 0;

    //退出 & 执行回调
    virtual void exitTask(std::function<void(std::list<core::UTask> &)> callback) = 0;

    //virtual void exitTask(std::function<void(std::list<core::UTask> &)> queueCb, TETaskBlock excuteBlock) = 0;

    virtual void clearTask() = 0;

    const std::string &getThreadName() const { return m_sName;}

protected:
    IUThread *mThread;
    std::mutex mMutexPool;
    pthread_mutex_t mTaskQueueMutex;
    pthread_cond_t mTaskQueueCond;
    std::list<core::UTask> mLowPriorityList;
    std::list<UTaskBlock> mHighPriorityList;

    std::string m_sName;

    bool mStop;
    bool mActive;
    bool mIsValid;
    int mMaxTaskCount;

    std::atomic<bool> mbExitAsyncTask;

};
