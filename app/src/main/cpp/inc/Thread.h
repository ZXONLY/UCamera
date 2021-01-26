//
// Created by 张翔 on 1/3/21.
//
#pragma once

#include <string>
#include <queue>
#include <memory>
#include <pthread.h>
#include <mutex>
#include <functional>

namespace basic {

    using TE_THREAD_STATE = enum {
        TE_THREAD_STATE_UNINIT = 0x0,
        TE_THREAD_STATE_IDLE,
        TE_THREAD_STATE_RUNNING,
        TE_THREAD_STATE_RUNEND,
        TE_THREAD_STATE_ERROR,
    };

    class Thread{
    public:

        typedef std::function< void()> func;

        explicit Thread();
        virtual ~Thread();
        virtual void runTask(func&);//往任务队列里塞任务
        virtual int join();
        virtual bool start();
        TE_THREAD_STATE getState();


    private:
        static void *sThreadFunc(void *arg);

    protected:
        void setState(TE_THREAD_STATE state);
        virtual void *threadFunc();
        virtual void didExitTask();//TODO 任务完成后进程回调，用于mark finish，区别于after run

    protected:
        pthread_t mThreadId;
        TE_THREAD_STATE mState;
        bool mExitFlag;
        pthread_mutex_t mStateMutex;
        pthread_cond_t mStateCond;
        pthread_cond_t mWaitTaskCond;
        pthread_mutex_t mTaskMutex;
        std::queue<func> mTaskQueue;

    };

}   // namespace basic
