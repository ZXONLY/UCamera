//
// Created by bytedance on 2020/12/29.
//
#include "LogUtils.h"
#include "Thread.h"


basic::Thread::Thread(){
    //mTaskQueue = new std::queue<func>();
    pthread_mutex_init(&mStateMutex, nullptr);
    pthread_cond_init(&mStateCond, nullptr);
    mState = TE_THREAD_STATE_UNINIT;
    pthread_cond_init(&mWaitTaskCond, nullptr);
    pthread_mutex_init(&mTaskMutex, nullptr);
}

basic::Thread::~Thread(){
    if(mState != TE_THREAD_STATE_UNINIT){
        mExitFlag = true;
        pthread_mutex_lock(&mTaskMutex);
        pthread_cond_signal(&mWaitTaskCond);
        pthread_mutex_unlock(&mTaskMutex);
        pthread_join(mThreadId, nullptr);
        mState = TE_THREAD_STATE_UNINIT;
    }
    pthread_cond_destroy(&mWaitTaskCond);
    pthread_mutex_destroy(&mTaskMutex);
    pthread_mutex_destroy(&mStateMutex);
    pthread_cond_destroy(&mStateCond);
}

bool basic::Thread::start() {
    int ret = pthread_create(&mThreadId, nullptr,sThreadFunc,this);
    if(ret < 0){
        return false;
    }
    pthread_mutex_lock(&mStateMutex);
    while(mState == TE_THREAD_STATE_UNINIT){
        pthread_cond_wait(&mStateCond,&mStateMutex);
    }
    pthread_mutex_unlock(&mStateMutex);
    return true;
}

int basic::Thread::join() {//TODO: 感觉有点问题，真的停住了嘛
    if(getState() == TE_THREAD_STATE_RUNNING){
        pthread_mutex_lock(&mStateMutex);
        while(mState == TE_THREAD_STATE_RUNNING){
            pthread_cond_wait(&mStateCond,&mStateMutex);
        }
        pthread_mutex_unlock(&mStateMutex);
    }
    setState(TE_THREAD_STATE_IDLE);
    return 0;
}
basic::TE_THREAD_STATE basic::Thread::getState() {
    TE_THREAD_STATE ret = TE_THREAD_STATE_UNINIT;
    pthread_mutex_lock(&mStateMutex);
    ret = mState;
    pthread_mutex_unlock(&mStateMutex);
    return ret;
}
void basic::Thread::setState(TE_THREAD_STATE state) {
    pthread_mutex_lock(&mStateMutex);
    mState = state;
    pthread_cond_signal(&mStateCond);
    pthread_mutex_unlock(&mStateMutex);
}

void basic::Thread::runTask(func& test) {//TODO 要修改昂
    if(getState() == TE_THREAD_STATE_IDLE){
        pthread_mutex_lock(&mTaskMutex);
        mTaskQueue.push(std::move(test));
        pthread_cond_signal(&mWaitTaskCond);
        pthread_mutex_unlock(&mTaskMutex);
    }
}

void *basic::Thread::sThreadFunc(void *arg) {
    auto *thiz = (Thread *)arg;
//TODO
//#if defined(__ANDROID__)
//    JNIEnv *pEnv = nullptr;
//    TE_JNI_GetJNIEnv(pEnv);
//#endif
    return thiz->threadFunc();
}

void *basic::Thread::threadFunc() {
    setState(TE_THREAD_STATE_IDLE);
    pthread_mutex_lock(&mTaskMutex);
    while(!mExitFlag){
        while(mTaskQueue.empty()){
            pthread_cond_wait(&mWaitTaskCond,&mTaskMutex);
        }
        func &test = mTaskQueue.front();
        mTaskQueue.pop();
        pthread_mutex_unlock(&mTaskMutex);
        setState(TE_THREAD_STATE_RUNNING);
        test();
        setState(TE_THREAD_STATE_RUNEND);
        didExitTask();
    }
    return nullptr;
}

void basic::Thread::didExitTask() {
    LOGD("didExitTask");
}



