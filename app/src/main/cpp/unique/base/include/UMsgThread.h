////
//// Created by bytedance on 10/03/2021.
////
//
//#ifndef CAMERA_UMSGTHREAD_H
//#define CAMERA_UMSGTHREAD_H
//
//#endif //CAMERA_UMSGTHREAD_H
//
//#pragma once
//
//#include <pthread.h>    // not platform dependent
//#include "TEMessageQueue.h"
//#include "TEResult.h"
//#include "TEThreadPool.h"
//
//
//class TEThreadObject {
//public:
//    TEThreadObject();
//    virtual ~TEThreadObject();
//
//public:
//    virtual void postMessage(TEMsg &msg);   // called from other thread
//    void insertMessage(TEMsg &msg);
//    void insertAndPost(TEMsg& msgInsert, TEMsg& msgPost);
//    void insertMessageAfter(TEMsg &msg, int msgWhat);
//    void clearMessageQueue();
//    void clearMessageQueue(int msgType);
//    int clearMessageQueue(std::function<bool(int msgWhat)> funcMsgToClear);
//    int clearMessageQueueList(std::function<int(list<TEMsg> &queueMsg)> funcMsgToClear);
//    int putMessage(std::function<int(list<TEMsg> &queueMsg,const TEMsg &targetMsg)> funcMsgToPut,const TEMsg &msg);
//    int putMessageUnsafe(std::function<int(list<TEMsg> &queueMsg, const TEMsg &targetMsg)> funcMsgToPut, const TEMsg &msg);
//    void clearDataMessage();
//    void stopThread();
//    void blockThread();
//    void activateThread();
//    void setThreadName(const char *name);
//    string getThreadName();
//    bool startThread();
//    const bool isActive() const { return m_isActive; }
//
//    pthread_id_t getWordThreadTid() { return  m_tid;}
//    TEResult threadLoop();
//
//protected:
//    // classes want to process message queue must override this method
//    virtual TEResult customEvent(TEMsg *) = 0;
//    virtual void getThreadAttr(TEThreadAttr *attr);
//    virtual int  pthreadCondWait(pthread_cond_t* cond, pthread_mutex_t* mutex);
//protected:
//    static void * threadStartCallback(void *arg);
//    int getMessageQueueSize();
//    void debugPrintMessageQueue();
//
//protected:
//    TEMessageQueue          m_queueMessage;
//    //pthread_t               m_lThreadId;
//    TEThread                *m_threadId;
//    bool                    m_bRun;
//    bool                    m_isActive;
//    bool                    m_threadFailed;
//    pthread_mutex_t         m_oLock;
//    pthread_cond_t          m_oCondition;
//    string                  m_sName;
//    pthread_id_t            m_tid;
//};
