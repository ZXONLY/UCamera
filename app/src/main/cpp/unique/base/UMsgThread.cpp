////
//// Created by bytedance on 10/03/2021.
////
//
//#include "include/UMsgThread.h"
//#if !defined(_WIN32)
//#include <unistd.h>
//#endif
//#include "TEThreadObject.h"
//#ifdef __wasm__
//#include <emscripten/emscripten.h>
//#include "TEStreamingDefine.h"
//#endif
//#if defined(__linux__)
//
//#include <unistd.h>
//#include <sys/syscall.h>
//
//#endif
//#ifdef TAG
//#undef TAG
//#endif
//#define TAG "TEThreadObject"
//
//TEThreadObject::TEThreadObject()
//        : m_threadId(nullptr), m_bRun(true), m_isActive(true), m_threadFailed(false) {
//    m_queueMessage.clear();
//
//    pthread_mutex_init(&m_oLock, NULL);
//    pthread_cond_init(&m_oCondition, NULL);
//    //pthread_create(&m_lThreadId, NULL, threadStartCallback, this);
//}
//
//TEThreadObject::~TEThreadObject() {
//    stopThread();
//
//    pthread_mutex_destroy(&m_oLock);
//    pthread_cond_destroy(&m_oCondition);
//}
//
//bool TEThreadObject::startThread() {
//    if (m_threadFailed) {
//        return false;
//    }
//    TEThreadAttr attr;
//    getThreadAttr(&attr);
//    if (!m_threadId) {
//        m_threadId = TEThreadPool::getInstance()->allocThreadAndRun(threadStartCallback, this, &attr);
//    }
//    if (attr.extraData != nullptr) {
//        free(attr.extraData);
//        attr.extraData = nullptr;
//    }
//    if (!m_threadId) {
//        m_threadFailed = true;
//    } else {
//        m_threadId->setName(m_sName);
//    }
//    return m_threadId != nullptr;
//}
//
//
//void * TEThreadObject::threadStartCallback(void *arg) {
//
//    TEThreadObject * pointer = (TEThreadObject*)arg;
//    TEResult ret = pointer->threadLoop();
//    VELOGW("threadobject:%p exit thread: %s", (void *)pointer, pointer->getThreadName().c_str());
//    //    if (ret == TER_EXIT)
//    //        delete pointer;
//    // pthread_exit(NULL);
//    return NULL;
//}
//
//void TEThreadObject::blockThread() {
//    pthread_mutex_lock(&m_oLock);
//    m_isActive = false;
//    pthread_cond_signal(&m_oCondition);
//    pthread_mutex_unlock(&m_oLock);
//}
//
//void TEThreadObject::activateThread() {
//    pthread_mutex_lock(&m_oLock);
//    m_isActive = true;
//    pthread_cond_signal(&m_oCondition);
//    pthread_mutex_unlock(&m_oLock);
//}
//
//void TEThreadObject::stopThread() {
//
//    if (!m_threadId)
//        return;
//
//    pthread_mutex_lock(&m_oLock);
//    m_bRun = false;
//    VELOGW("threadobject:%p, %s %d %s", (void *)this, __FUNCTION__, __LINE__, m_sName.c_str());
//    pthread_cond_signal(&m_oCondition);
//    pthread_mutex_unlock(&m_oLock);
//    pthread_id_t tid;
//#if defined(__ANDROID__) || defined(__IOS__)
//    tid = gettid();
//#elif defined(__linux__)
//    tid = pthread_self();
//#elif defined(_WIN32)
//    tid = pthread_getw32threadid_np(pthread_self());
//#else
//    tid = gettid();
//#endif
//    if (m_threadId && m_tid != tid) {
//        m_threadId->join();
//        m_threadId = nullptr;
//    }
//    //pthread_join(m_lThreadId, NULL);
//}
//
//
//TEResult TEThreadObject::threadLoop() {
//
//    m_threadId->setRunningThreadName(m_sName);
//
//#if defined(__ANDROID__) || defined(__IOS__)
//    m_tid = gettid();
//#elif defined(__linux__)
//    m_tid = pthread_self();
//#elif defined(_WIN32)
//    m_tid = pthread_getw32threadid_np(pthread_self());
//#else
//    m_tid = gettid();
//#endif
//    auto oldname = m_threadId->getThreadName();
//    m_threadId->setThreadName(m_sName);
//    while (m_bRun) {
//        TEMsg msg;
//        pthread_mutex_lock(&m_oLock);
//        if (!m_bRun) {
//            pthread_mutex_unlock(&m_oLock);
//            break;
//        }
//
//        if (!m_isActive) {
//            VELOGD("TEThreadObject::threadLoop() %d %s", __LINE__, m_sName.c_str());
//            if (!(m_queueMessage.front(msg) && msg.forceExecute)) {
//                pthreadCondWait(&m_oCondition, &m_oLock);
//                pthread_mutex_unlock(&m_oLock);
//                continue;
//            }
//        }
//        if (!m_queueMessage.pop(msg)) {
//            pthreadCondWait(&m_oCondition, &m_oLock);
//            pthread_mutex_unlock(&m_oLock);
//            continue;
//        } else {
//            pthread_mutex_unlock(&m_oLock);
//        }
//
//        if (!m_bRun)
//            break;
//
//        if (TER_EXIT == customEvent(&msg)) {
//            return TER_EXIT;
//        }
//        msg.what = -1;
//    }
//    m_threadId->setThreadName(oldname);
//    return TER_OK;
//}
//
//int  TEThreadObject::pthreadCondWait(pthread_cond_t* cond, pthread_mutex_t* mutex) {
//    return pthread_cond_wait(cond, mutex);
//}
//
//int TEThreadObject::getMessageQueueSize() {
//    return m_queueMessage.size();
//}
//
//void TEThreadObject::debugPrintMessageQueue() {
//    pthread_mutex_lock(&m_oLock);
//    m_queueMessage.debugPrint();
//    pthread_mutex_unlock(&m_oLock);
//}
//
//void TEThreadObject::postMessage(TEMsg& msg) {
//    if (!m_bRun)
//        return;
//    pthread_mutex_lock(&m_oLock);
//    if (!m_threadId)
//        startThread();
//    m_queueMessage.push(msg);
//    if (m_queueMessage.size() > 0)
//        pthread_cond_signal(&m_oCondition);
//    pthread_mutex_unlock(&m_oLock);
//}
//
//
//void TEThreadObject::insertMessage(TEMsg &msg) {
//    if (!m_bRun)
//        return;
//    pthread_mutex_lock(&m_oLock);
//    if (!m_threadId)
//        startThread();
//    m_queueMessage.insert(msg);
//    if (m_queueMessage.size() > 0)
//        pthread_cond_signal(&m_oCondition);
//    pthread_mutex_unlock(&m_oLock);
//}
//
//void TEThreadObject::insertAndPost(TEMsg &msgInsert, TEMsg &msgPost) {
//    if (!m_bRun)
//        return;
//    pthread_mutex_lock(&m_oLock);
//    if (!m_threadId)
//        startThread();
//    m_queueMessage.insert(msgInsert);
//    m_queueMessage.push(msgPost);
//    if (m_queueMessage.size() > 0)
//        pthread_cond_signal(&m_oCondition);
//    pthread_mutex_unlock(&m_oLock);
//}
//
//void TEThreadObject::insertMessageAfter(TEMsg &msg, int msgWhat) {
//    if (!m_bRun)
//        return;
//    pthread_mutex_lock(&m_oLock);
//    if (!m_threadId)
//        startThread();
//    m_queueMessage.insertAfter(msg, msgWhat);
//    if (m_queueMessage.size() > 0)
//        pthread_cond_signal(&m_oCondition);
//    pthread_mutex_unlock(&m_oLock);
//}
//
//void TEThreadObject::clearMessageQueue() {
//    pthread_mutex_lock(&m_oLock);
//    m_queueMessage.clear();
//    pthread_mutex_unlock(&m_oLock);
//}
//
//void TEThreadObject::clearMessageQueue(int msgType) {
//    pthread_mutex_lock(&m_oLock);
//    m_queueMessage.clear(msgType);
//    pthread_mutex_unlock(&m_oLock);
//}
//
//int TEThreadObject::clearMessageQueue(std::function<bool(int msgWhat)> funcMsgToClear) {
//    int clear_cnt = 0;
//    pthread_mutex_lock(&m_oLock);
//    clear_cnt = m_queueMessage.clear(funcMsgToClear);
//    pthread_mutex_unlock(&m_oLock);
//
//    return clear_cnt;
//}
//
//void TEThreadObject::setThreadName(const char *name) {
//    if (name) {
//        //pthread_setname_np(m_lThreadId, name);
//        m_sName = name;
//        if (m_threadId) {
//            m_threadId->setName(name);
//        }
//    }
//}
//
//string TEThreadObject::getThreadName() {
//    return m_sName;
//}
//
//void TEThreadObject::clearDataMessage() {
//    auto funcMsgToClear = [](int msgWhat)->bool { return ((msgWhat >> 24) & 0xFF) != 'C';};
//    clearMessageQueue(funcMsgToClear);
//}
//
//void TEThreadObject::getThreadAttr(TEThreadAttr *attr) {
//    if (attr != nullptr) {
//        attr->extraData = nullptr;
//        attr->type = TE_THREAD_TYPE_NORMAL;
//        attr->name = "TEThreadObject";
//    }
//}
//
//int TEThreadObject::clearMessageQueueList(std::function<int(list<TEMsg> &queueMsg)> funcMsgToClear) {
//    pthread_mutex_lock(&m_oLock);
//    m_queueMessage.clear(funcMsgToClear);
//    pthread_mutex_unlock(&m_oLock);
//    return 0;
//}
//
//int TEThreadObject::putMessage(std::function<int(list<TEMsg> &queueMsg,const TEMsg &targetMsg)> funcMsgToPut,const TEMsg &msg) {
//pthread_mutex_lock(&m_oLock);
//int ret = m_queueMessage.put(funcMsgToPut,msg);
//pthread_mutex_unlock(&m_oLock);
//return ret;
//}