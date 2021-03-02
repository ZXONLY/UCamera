//
// Created by bytedance on 28/10/2020.
//

#ifndef CAMERA_EGLTHREAD_H
#define CAMERA_EGLTHREAD_H

#include <pthread.h>
#include <android/native_window.h>
#include <EGL/eglplatform.h>
#include <unistd.h>
class EGLThread{
public:
    pthread_t mEGLThread = -1;

    pthread_mutex_t pthread_mutex;
    pthread_cond_t pthread_cond;
public:
    EGLThread();
    ~EGLThread();
    void onSurfaceCreate(EGLNativeWindowType window);
    void onSurfaceChange(int width,int height);
    void onSurfaceDestroyed();
    void setWidthandHeight(int width,int Height);

    typedef void(*OnCreate)();
    OnCreate onCreate;
    typedef void(*OnChange)(int width,int height);
    OnChange onChange;

    typedef void(*OnDraw)();
    OnDraw onDraw;

    void setonCreateCallback(OnCreate onCreate1);
    void setonChangeCallback(OnChange onChange1);
    void setonDrawCallback(OnDraw onDraw1 );

    void notifyRender();



    ANativeWindow *mANativeWindow = nullptr;
    bool isCreate = false;
    bool isChange = false;
    bool isExit = false;
    bool isStart = true;

    int surfaceWidth = -1;
    int surfaceHeight = -1;
    //int texture = -1;
};
#endif //CAMERA_EGLTHREAD_H
