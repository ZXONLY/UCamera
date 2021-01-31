//
// Created by bytedance on 28/10/2020.
//
#include <LogUtils.h>
#include "../OpenGLUtils/include/EGLThread.h"
#include "../OpenGLUtils/include/EGLHelper.h"

extern "C"{
#include "../ffmpeg/include/libavutil/avutil.h"
}


EGLThread::EGLThread(){
    pthread_mutex_init(&pthread_mutex, nullptr);
    pthread_cond_init(&pthread_cond, nullptr);
    onDraw = nullptr;
    onCreate = nullptr;
    onChange = nullptr;
}

EGLThread::~EGLThread() {
    isExit = false;
    pthread_join(mEGLThread, nullptr);
    pthread_mutex_destroy(&pthread_mutex);
    pthread_cond_destroy(&pthread_cond);
}

void *eglThreadImpl(void *context){
    EGLThread *eglThread = static_cast<EGLThread *>(context);
    if(!eglThread){
        LOGE("eglThreadImpl eglThread is null");
        return 0;
    }

    EGLHelper *eglHelper = new EGLHelper();
    eglHelper->initEGL();
    if(!eglHelper->render(eglThread->mANativeWindow)){
        LOGE("eglHelper initEgl error");
        return 0;
    }
    LOGD("%s",av_version_info())
    eglThread->isExit = true;
    while(eglThread->isExit){
        if(!eglThread->isCreate){
            eglThread->isCreate = true;
            LOGD("onCreate");
            eglThread->onCreate();
        }

        if(!eglThread->isChange){
            eglThread->isChange = true;
            eglThread->onChange(eglThread->surfaceWidth,eglThread->surfaceHeight);
        }

        if(eglThread->isStart){
            LOGI("hhhhI draw");
            eglThread->onDraw();
            eglHelper->swapBuffers();

            if(false){
                pthread_mutex_lock(&eglThread->pthread_mutex);
                pthread_cond_wait(&eglThread->pthread_cond,&eglThread->pthread_mutex);
                pthread_mutex_unlock(&eglThread->pthread_mutex);
            } else{
                usleep(100000 / 6);
            }
        }
    }
    eglHelper->releaseEGL();
    //delete eglHelper;
    eglHelper = NULL;
    eglThread->isCreate = false;
    //return 0表示线程结束
    return 0;
}

void EGLThread::onSurfaceCreate(EGLNativeWindowType window) {
    if(mEGLThread==-1){
        isCreate = false;
        mANativeWindow = window;
        pthread_create(&mEGLThread, NULL, eglThreadImpl, this);
    }
}

void EGLThread::onSurfaceChange(int width, int height) {
    if(mEGLThread!=-1){
        surfaceHeight = height;
        surfaceWidth = width;
        isChange = false;

        notifyRender();
    }
}


void EGLThread::notifyRender() {
    pthread_mutex_lock(&pthread_mutex);
    pthread_cond_signal(&pthread_cond);
    pthread_mutex_unlock(&pthread_mutex);
}

void EGLThread::setonDrawCallback(OnDraw onDraw1) {
    onDraw = onDraw1;
}

void EGLThread::setonChangeCallback(OnChange onChange1) {
    onChange = onChange1;
}

void EGLThread::setonCreateCallback(OnCreate onCreate1) {
    onCreate = onCreate1;
}
void EGLThread::setWidthandHeight(int width, int height) {
    surfaceWidth = width;
    surfaceHeight = height;
}
void EGLThread::onSurfaceDestroyed(){
    isExit = false;
}