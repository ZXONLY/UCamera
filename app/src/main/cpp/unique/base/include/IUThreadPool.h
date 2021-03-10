//
// Created by bytedance on 02/03/2021.
//

#pragma once

#include <UDef.h>
#include "../../include/UResult.h"
#include <functional>
#include "UThreadId.h"
#include <string>
#include <UMsg.h>
#include <unordered_map>


using IUTask = std::function<void(UMsg)>;

UInterface IUThread{
    virtual UResult syncTask(IUTask task) = 0;
    virtual UResult asyncTask(IUTask task) = 0;
    virtual UResult runTask(IUTask task) = 0;
};

UInterface IUThreadPool{
    virtual UResult creatThread(IUThread **pthread) = 0;

    virtual int checkThreadCount() = 0;

    virtual int recycle() = 0;
};


class UThread {
public:
    explicit UThread(TEThreadPool *pool);
    virtual ~UThread();

public:
    virtual bool start();
    pthread_id_t getTid();
    virtual int join();
    U_THREAD_STATE getState();
    virtual int runTask(IUTask task,const UMsg& msg, const std::string &name);
    std::string getCurTaskName();
    void setName(const std::string &name);
    void setRunningThreadName(const std::string &name);
    U_THREAD_TYPE getType();
    bool acquireSelfUse();
    void releaseSelfUse();
    bool useSelf();

    void setThreadName(std::string const& name);
    std::string getThreadName() const;

protected:
    virtual bool onThreadEnter() {return true;}
    virtual void onThreadExit() {}
    virtual bool willEnterTask() {return true;}
    virtual void didExitTask() {}
    virtual void *threadFunc();
    void setState(U_THREAD_STATE state);

private:
    static void *sThreadFunc(void *arg);

protected:
    U_THREAD_TYPE mType;
    pthread_id_t mTid;
    bool mUseSelf;
    TEThreadPool *mThreadPool;

protected:
    void *(*mThreadFunc)(void *);
    void *mThreadArg;
    std::string mTaskName;
    pthread_t mThreadid;
    pthread_mutex_t mStateMutex;
    pthread_cond_t mStateCond;
    pthread_cond_t mWaitTaskCond;
    pthread_mutex_t mTaskMutex;
    bool mExitFlag;
    U_THREAD_STATE mState;
};
//
//class TEThreadPool {
//public:
//    TEThreadPool();
//    ~TEThreadPool();
//
//public:
//    static TEThreadPool *getInstance();
//    TEThread *allocThreadAndRun(TEThreadFunc threadFunc, void *arg, const TEThreadAttr *threadAttr,
//                                bool useMulti = false);
//    bool setMaxThreadCount(int count);
//    void checkThreadCount(TEThread *caller);
//
//private:
//    std::list<TEThread *>mThreads;
//    std::mutex mThreadMutex;
//    static TEThreadPool *sSharedInstance;
//    static std::mutex sSharedInstanceMutex;
//    int mMaxThreadCount;
//
//    EGLDisplay m_display = EGL_NO_DISPLAY;
//};
//
//class TERecordThreadPool {
//
//public:
//    static TERecordThreadPool *createThreadPool() { return new TERecordThreadPool(); }
//
//    TEGLTaskQueue *getThread(kernel::TEThreadDesc desc);
//
//    std::unique_ptr<TETaskQueue> allocDetachedTaskQueue(int maxNum = 1);
//
//    void allocGLTaskQueue(kernel::TEThreadDesc desc, TEThreadAttr attr, int mMaxTaskCount);
//
//    void attachGlobalGLThread(kernel::TEThreadDesc desc, TEGLTaskQueue *thread);
//
//    std::unordered_map<kernel::TEThreadDesc, TEGLTaskQueue *, kernel::EnumClassHash> &getThreadPoolMap() { return m_threadPool; }
//    ~TERecordThreadPool();
//
//    TERecordThreadPool() {};
//
//private:
//    TERecordThreadPool(const TERecordThreadPool &);
//    TERecordThreadPool &operator=(const TERecordThreadPool &);
//
//    //std::unordered_map<kernel::TEThreadDesc, TEGLTaskQueue *, kernel::EnumClassHash> m_threadPool;
//    //TEGLTaskQueue *mResourceManagerProcessQueue = nullptr;
//};