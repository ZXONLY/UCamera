//
// Created by 张翔 on 2/17/21.
//

#include "UGLShareContext.h"
#include <UResult.h>
#include "LogUtils.h"

EGLint UGLShareContext::s_bitR = 8;
EGLint UGLShareContext::s_bitG = 8;
EGLint UGLShareContext::s_bitB = 8;
int UGLShareContext::s_nGLVersion = 3;
bool UGLShareContext::s_checkGLVersion3 = true;
USizei UGLShareContext::m_maxRenderSize(0,0);

void UGLShareContext::setGLVersion(int nVersion) {
    s_nGLVersion = nVersion;
}

void UGLShareContext::setContextColorBits(int r, int g, int b) {
    s_bitB = b;
    s_bitG = g;
    s_bitR = r;
}

void UGLShareContext::setMaxRenderSize(int width, int height) {
    m_maxRenderSize.set(width,height);
}

UGLShareContext *UGLShareContext::create() {
    UGLShareContext *context = new UGLShareContext();
    if(context->init(EGL_NO_CONTEXT,64,64,TYPE_PIXEL_BUFFER) != UR_OK){
        delete context;
        context = nullptr;
    }
    return context;
}

UGLShareContext *UGLShareContext::create(int width, int height) {
    UGLShareContext *context = new UGLShareContext();
    if(context->init(EGL_NO_CONTEXT,width,height,TYPE_PIXEL_BUFFER) != UR_OK){
        delete context;
        context = nullptr;
    }
    return context;
}

UGLShareContext *UGLShareContext::create(EGLContext sharedContext, int nWidth, int nHeight,
        EEGLConfigType type, EGLNativeWindowType win) {
    UGLShareContext *context = new UGLShareContext();
    if(context->init(sharedContext,nWidth,nHeight,type,win) != UR_OK){
        delete context;
        context = nullptr;
    }
    return context;
}

UGLShareContext *UGLShareContext::create(EGLContext sharedContext, int nWidth, int nHeight,
                                         EEGLConfigType type) {
    UGLShareContext *context = new UGLShareContext();
    if(context->init(sharedContext,nWidth,nHeight,type) != UR_OK){
        delete context;
        context = nullptr;
    }
    return context;
}

UGLShareContext::UGLShareContext(){
    initData();
}
UGLShareContext::~UGLShareContext(){
    release();
}

int UGLShareContext::init(EGLContext sharedContext, int nWidth, int nHeight, EEGLConfigType type, EGLNativeWindowType win) {
    if (sharedContext == nullptr){
        sharedContext = EGL_NO_CONTEXT;
    }

    int ret = initContext(sharedContext, type);
    if(ret != UR_OK){
        ret = ret;
    }

    //m_nAndroidVersion =  getOSVersion();

    int clientVersion ;
    eglQueryContext(m_display,m_context,EGL_CONTEXT_CLIENT_VERSION, &clientVersion);
    LOGD("EGLCONTECT created, client version %d\n",clientVersion);

    if(m_pFrameBuffer == nullptr){
        makeCurrent(m_surface);
        //check gl init

        m_pFrameBuffer = std::make_shared<UFrameBuffer>();
        LOGD("FrameBuffer: %p, id = %u", (void *)m_pFrameBuffer.get(), m_pFrameBuffer->getID());
        doneCurrent();
    }

    return UR_OK;
}

void UGLShareContext::release() {
    if(!isValid()){
        LOGD("GLCONTEXT is not ready")
        return;
    }
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    if(m_pFrameBuffer){
        LOGD("FrameBuffer: %p, id = %u", (void *)m_pFrameBuffer.get(), m_pFrameBuffer->getID());
        m_pFrameBuffer = nullptr;
    }

//    if (m_nAndroidVersion == 21 || m_nAndroidVersion == 22) {
//        LOGE("call glFinish version %d", m_nAndroidVersion);
//        glFinish();
//    }

    if (m_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (m_context != EGL_NO_CONTEXT) {
            eglDestroyContext(m_display, m_context);
            //CHECK_GL_ERROR_FORCE("release eglDestroyContext");
        }

        if (m_surface != EGL_NO_SURFACE) {
            eglDestroySurface(m_display, m_surface);
            //CHECK_GL_ERROR_FORCE("release eglDestroySurface");
        }

        eglTerminate(m_display);
        //CHECK_GL_ERROR_FORCE("release eglTerminate display");
    }

    m_display = EGL_NO_DISPLAY;
    m_surface = EGL_NO_SURFACE;
    m_context = EGL_NO_CONTEXT;
}

bool UGLShareContext::makeCurrent(EGLSurface surface) {
    if (surface == EGL_NO_SURFACE) surface = m_surface;
    if (surface == EGL_NO_SURFACE) {
        LOGE("Invalid Surface!");
        return false;
    }
    if(m_context == nullptr) {
        LOGE("m_context is nullptr!");
        return false;
    }
    if (!eglMakeCurrent(m_display, surface, surface, m_context)) {
        LOGE("eglMakeCurrent failed: 0x%x", eglGetError());
        return false;
    }
    return true;
}

EGLBoolean UGLShareContext::swapBuffers() {
    return eglSwapBuffers(m_display,m_surface);
}

int UGLShareContext::initContext(EGLContext sharedContext, EEGLConfigType type) {
    int nGLRendertype = 0;

    if((m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY){
        LOGE("eglGetDisplay() returned error 0x%x",eglGetError())
        return UR_EGL_BAD_DISPLAY;
    }

    EGLint major,minor;

    if(!eglInitialize(m_display,&major,&minor)){
        LOGE("eglInitialize() return error 0x%x",eglGetError())
        return UR_EGL_BAD_DISPLAY;
    }

    LOGD("eglInitialize major: %d, minor: %d",major,minor)
    if(minor >= 4 && s_nGLVersion >= 3){
        nGLRendertype = EGL_OPENGL_ES3_BIT_KHR;
    } else {
        nGLRendertype = EGL_OPENGL_ES2_BIT;
        s_nGLVersion = 2;
    }

    EGLint configSpec[] = {
            EGL_RED_SIZE, s_bitR,
            EGL_GREEN_SIZE, s_bitG,
            EGL_BLUE_SIZE, s_bitB,
            EGL_ALPHA_SIZE, s_bitA,
            EGL_RENDERABLE_TYPE, nGLRendertype,
            EGL_NONE, 0,
            EGL_NONE
    };

    const int configSize = sizeof(configSpec) / sizeof(*configSpec);

    switch (type) {
        case TYPE_PIXEL_BUFFER:
            configSpec[configSize - 3] = EGL_SURFACE_TYPE;
            configSpec[configSize - 2] = EGL_PBUFFER_BIT;
            break;

        case TYPE_WINDOW_BIT:
            configSpec[configSize - 3] = EGL_SURFACE_TYPE;
            configSpec[configSize - 2] = EGL_WINDOW_BIT;
            break;

        case TYPE_PIX_MAP:
            configSpec[configSize - 3] = EGL_SURFACE_TYPE;
            configSpec[configSize - 2] = EGL_PIXMAP_BIT;
            break;

        case TYPE_RECORDABLE:
            configSpec[configSize - 3] = EGL_RECORDABLE_ANDROID;
            configSpec[configSize - 2] = EGL_TRUE;
            break;

        default:
            LOGE("Invalid config type = %d", (int)type);
            return UR_INVALID_PARAM;
    }

    EGLConfig config;
    EGLint numConfigs;

    LOGD("Initializing context");

    if (!eglChooseConfig(m_display, configSpec, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig() returned error 0x%x", eglGetError());
        release();
        return UR_EGL_BAD_CONFIG;
    }

    m_config = config;
    EGLint attribList[]{
            EGL_CONTEXT_CLIENT_VERSION, s_nGLVersion,
            EGL_NONE
    };
    if (sharedContext != nullptr)
        LOGD("Has shared context, %p", sharedContext);
    m_context = eglCreateContext(m_display, config, sharedContext, attribList);
    if (m_context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext Failed:  0x%x", eglGetError());
        return UR_EGL_BAD_CONTEXT;
    }

    return UR_OK;
}

int UGLShareContext::initSurface(int nWidth, int nHeight, UGLShareContext::EEGLConfigType eType,
                                   EGLNativeWindowType win) {
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLint pBufferAttrib[] = {
            EGL_WIDTH, nWidth,
            EGL_HEIGHT, nHeight,
            EGL_LARGEST_PBUFFER, EGL_TRUE,
            EGL_NONE
    };

    switch (eType) {
        case TYPE_RECORDABLE:
        case TYPE_PIXEL_BUFFER:
            eglSurface = eglCreatePbufferSurface(m_display, m_config, pBufferAttrib);
            break;

        case TYPE_WINDOW_BIT:
            pBufferAttrib[4] = EGL_NONE;
            pBufferAttrib[5] = EGL_NONE;
            eglSurface = eglCreateWindowSurface(m_display, m_config, win, NULL);
            break;

        case TYPE_PIX_MAP:
            eglSurface = eglCreatePixmapSurface(m_display, m_config, (EGLNativePixmapType) win,
                                                pBufferAttrib);
            break;

        default:
            LOGE("Invalid config type = %d", (int)eType);
            return UR_INVALID_PARAM;
    }

    if (eglSurface == EGL_NO_SURFACE) {
        LOGE("Create Surface Failed:  0x%x", eglGetError());
        return UR_EGL_BAD_SURFACE;
    }

    m_surface = eglSurface;

    return UR_OK;
}

int UGLShareContext::getSurfaceSize(int &nWidth, int &nHeight) {

    if (m_context == EGL_NO_CONTEXT || m_surface == EGL_NO_SURFACE || m_display == EGL_NO_DISPLAY) {
        return UR_EGL_ERROR;
    }

    if (!eglQuerySurface(m_display, m_surface, EGL_WIDTH, &nWidth)) {
        LOGE("Get Surface width failed");
        return UR_FAIL;
    }

    if (!eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &nHeight)) {
        LOGE("Get Surface width failed");
        return UR_FAIL;
    }
    return UR_OK;
}

void UGLShareContext::bind(const bool flag) {

}

void UGLShareContext::unbind() {

}
void UGLShareContext::readPixel(UBaseFrame *frame, void *pixelData){

};

void UGLShareContext::flush() {

}

void UGLShareContext::finish() {

}

//USizei &UGLShareContext::getMaxRenderSize() {
//    if (!m_maxRenderSize.isValid()) {
//        int size = 0;
//        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
//        m_maxRenderSize.set(size, size);
//    }
//
//    return m_maxRenderSize;
//}

void UGLShareContext::doneCurrent() {
    if (m_display == EGL_NO_DISPLAY) {
        return;
    }

    if (EGL_TRUE != eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        LOGE("%s:: eglMakeCurrent error 0x%x", __FUNCTION__, eglGetError());
    }
}

void UGLShareContext::releaseSurface(bool isCtxUnBind) {
    if (m_surface != EGL_NO_SURFACE) {
        if (isCtxUnBind) {
            eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        } else {
            eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, m_context);
        }
        eglDestroySurface(m_display, m_surface);
        m_surface = EGL_NO_SURFACE;
    }
}

bool UGLShareContext::isValid() {
    return m_context != EGL_NO_CONTEXT && m_display != EGL_NO_DISPLAY;
}

bool UGLShareContext::genDefaultFramebuffer() {
    if (!m_pFrameBuffer)
        m_pFrameBuffer = std::make_shared<UFrameBuffer>();
    return m_pFrameBuffer != nullptr;
}


int UGLShareContext::getEGLConfigAttrib(int attribute, int *value) {
    if (m_context == EGL_NO_CONTEXT || m_surface == EGL_NO_SURFACE || m_display == EGL_NO_DISPLAY) {
        return UR_EGL_ERROR;
    }
    if (value == nullptr) {
        return UR_INVALID_PARAM;
    }
    if (eglGetConfigAttrib(m_display, m_config, attribute, value) == EGL_TRUE) {
        return UR_OK;
    } else {
        return UR_FAIL;
    }
}

