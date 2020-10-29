//
// Created by bytedance on 28/10/2020.
//

#include "EGLHelper.h"
#include "../inc/LogUtils.h"



EGLHelper::EGLHelper() {
    //TODO
    mDisplay = EGL_NO_DISPLAY;
    mContext = EGL_NO_CONTEXT;
    mSurface = EGL_NO_SURFACE;
    mVersion = -1;

}

void EGLHelper::initEGL() {
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(mDisplay==EGL_NO_DISPLAY){
        LOGE("egl no display");
    }
    int version1 = 0;
    int version2 = 0;
    if(!eglInitialize(mDisplay,&version1,&version2)){
        mDisplay = EGL_NO_DISPLAY;
        LOGE("egl Initialize error");
    }

    getConfig(true);
    if(mConfig != nullptr){
        const EGLint contexAttribList[] = {EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
        mContext = eglCreateContext(mDisplay,mConfig,EGL_NO_CONTEXT,contexAttribList);
        if(mContext==EGL_NO_CONTEXT){
            LOGW("can't get egl version 3 %d",eglGetError());
        } else{
            mVersion = 3;
            LOGI("get egl version 3");
        }
    }
    if(mContext==EGL_NO_CONTEXT){
        getConfig(false);
        const EGLint attribList[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        mContext = eglCreateContext(mDisplay, mConfig, EGL_NO_CONTEXT, attribList);
        if(mContext==EGL_NO_CONTEXT){
            LOGE("can't get egl version 3 %d",eglGetError());
        } else{
            mVersion = 2;
        }
    }
}

void EGLHelper::getConfig(bool isGles3) {
    int rendertype = EGL_OPENGL_ES2_BIT;
    if(isGles3){
        rendertype = EGL_OPENGL_ES3_BIT;
    }
    EGLint numConfigs;
    const EGLint attribs[] = {
            EGL_RED_SIZE,8,
            EGL_GREEN_SIZE,8,
            EGL_BLUE_SIZE,8,
            EGL_ALPHA_SIZE,8,
            EGL_RENDERABLE_TYPE,rendertype,
            EGL_SURFACE_TYPE,EGL_PBUFFER_BIT,
            EGL_NONE
    };
    eglChooseConfig(mDisplay, attribs, &mConfig, 1, &numConfigs);
}

bool EGLHelper::render(ANativeWindow *surface) {
    const EGLint surfaceAttribList[] = {EGL_NONE};
    mSurface = eglCreateWindowSurface(mDisplay,mConfig,surface,surfaceAttribList);
    if(mSurface==EGL_NO_SURFACE){
        LOGI("I hava a problem");
        return false;
    }
    makeCurrent(mSurface);
    LOGI("any problem?");
    return true;
}

void EGLHelper::releaseSurface(EGLSurface eglSurface) {
    eglDestroySurface(mDisplay,eglSurface);
}

EGLSurface EGLHelper::createOffscreenSurface(int width, int height) {
    const EGLint surfaceAttribList[] = {
            EGL_WIDTH,width,
            EGL_HEIGHT,height,
            EGL_NONE
    };
    EGLSurface eglSurface = eglCreatePbufferSurface(mDisplay,mConfig,surfaceAttribList);
    if(eglSurface== EGL_NO_SURFACE){
        LOGE("can't get offscreen surface");
    }
    return eglSurface;
}

void EGLHelper::makeCurrent(EGLSurface eglSurface) {
    if(mDisplay==EGL_NO_DISPLAY){
        LOGD("NOTE: makeCurrent w/o display");
    }
    if(!eglMakeCurrent(mDisplay,eglSurface,eglSurface,mContext)){
        LOGE("eglMakeCurrent(draw,read) failed");
    }
}

void EGLHelper::makeCurrent(EGLSurface drawSurface, EGLSurface readSurface) {
    if(mDisplay==EGL_NO_DISPLAY){
        LOGD("NOTE: makeCurrent w/o display");
    }
    if(!eglMakeCurrent(mDisplay,drawSurface,readSurface,mContext)){
        LOGE("eglMakeCurrent(draw,read) failed");
    }
}

void EGLHelper::makeNothingCurrent() {
    if(!eglMakeCurrent(mDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,mContext)){
        LOGE("eglMakeCurrent(nothing) failed");
    }
}

bool EGLHelper::swapBuffers() {
    if(mDisplay!=EGL_NO_DISPLAY){
        return eglSwapBuffers(mDisplay,mSurface);
    } else{
        return false;
    }
}
void EGLHelper::releaseEGL(){
    if(mSurface== nullptr && mSurface != EGL_NO_SURFACE){
        makeNothingCurrent();
        releaseSurface(mSurface);
        mSurface = EGL_NO_SURFACE;
    }

    if(mContext!=EGL_NO_CONTEXT){
        eglDestroyContext(mDisplay,mContext);
        mContext = EGL_NO_CONTEXT;
    }
    if (mDisplay != nullptr) {
        eglTerminate(mDisplay);
        mDisplay = EGL_NO_DISPLAY;
    }
}