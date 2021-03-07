//
// Created by 张翔 on 2/17/21.
//
#pragma once

#include "GLHolder.h"
#include "UBaseFrame.h"
#include "USizei.h"
#include "UFrameBuffer.h"
#include <memory>

class UGLShareContext{
public:
    enum EEGLConfigType {
        TYPE_PIXEL_BUFFER = 0,      //Off screen surface
        TYPE_RECORDABLE,            //Off screen surface, especially used for android record surface
        TYPE_WINDOW_BIT,            //Window surface, normally render on screen
        TYPE_PIX_MAP,               //Pixel Map surface
    };
    static void setGLVersion(int nVersion);
    static int getGLVersion()  {return s_nGLVersion;};
    static bool isGLVersion3Support(){return s_nGLVersion > 2;};
    static void setContextColorBits(int r,int g, int b);

    static void setMaxRenderSize(int width, int height);

    static UGLShareContext *create();

    static UGLShareContext *create(int width, int height);

    static UGLShareContext *create(EGLContext sharedContext, int nWidth, int nHeight,
                                             EEGLConfigType type);

    static UGLShareContext *create(EGLContext sharedContext, int nWidth, int nHeight,EEGLConfigType type,
            EGLNativeWindowType win);

    UGLShareContext();

    ~UGLShareContext();

    int init(EGLContext sharedContext, int nWidth, int nHeight,EEGLConfigType type,
             EGLNativeWindowType win = 0);

    int initContext(EGLContext sharedContext, EEGLConfigType eType);

    int initSurface(int nWidth, int nHeight,EEGLConfigType eType, EGLNativeWindowType win);

    void releaseSurface(bool isCtxUnBind = true);

    void release();

    bool makeCurrent(EGLSurface surface = EGL_NO_SURFACE);

    void doneCurrent();

    EGLBoolean swapBuffers();

    bool isValid();

    int getSurfaceSize(int &nWidth, int &nHeight);

    void bind(const bool flag);

    void unbind();

    void readPixel(UBaseFrame *frame, void *pixelData);

    void flush();

    void finish();

    void initData(){};

    bool genDefaultFramebuffer();

    int getEGLConfigAttrib(int attribute, int *value);

    EGLContext getCurrentContext() const {}

private:
    static int s_nGLVersion;
    static bool s_checkGLVersion3;
    static GLint s_bitR, s_bitG, s_bitB, s_bitA;
    static USizei m_maxRenderSize;

    int m_nAndroidVersion = 0;
    EGLDisplay m_display = EGL_NO_DISPLAY;
    EGLSurface m_surface = EGL_NO_SURFACE;
    EGLConfig m_config = nullptr;
    EGLContext m_context = EGL_NO_CONTEXT;

    std::shared_ptr<UFrameBuffer> m_pFrameBuffer = nullptr;
    bool m_SurfaceD3DTexture2DShareHandle{ false };

};