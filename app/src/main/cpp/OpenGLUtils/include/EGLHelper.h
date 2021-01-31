//
// Created by bytedance on 28/10/2020.
//

#ifndef CAMERA_EGLHELPER_H
#define CAMERA_EGLHELPER_H

#include "../../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/android/native_window.h"
#include "../../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/EGL/eglplatform.h"
#include "../../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/EGL/egl.h"


class EGLHelper{
public:
    //static EGL* getInstance();
    bool render(ANativeWindow *);
    void releaseSurface(EGLSurface eglSurface);
    EGLSurface createOffscreenSurface(int width,int height);
    void makeCurrent(EGLSurface eglSurface);
    void makeCurrent(EGLSurface drawSurface,EGLSurface readSurface);
    void makeNothingCurrent();
    bool swapBuffers();
    int getGlVersion();
    void releaseEGL();
    void initEGL();
    EGLHelper();
    ~EGLHelper();
private:
    void getConfig(bool isGles3);
private:
    EGLDisplay mDisplay;
    EGLSurface mSurface;
    EGLContext mContext;
    EGLConfig mConfig;
    int mVersion;
    ANativeWindow *mWindow;
    //static EGLHelper* mEGL;
};

#endif //CAMERA_EGLHELPER_H
