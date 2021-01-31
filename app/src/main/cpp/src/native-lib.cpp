#include <jni.h>
#include "Trangles.h"
#include "../OpenGLUtils/include/EGLHelper.h"
#include "DrawerOES.h"
#include "Thread.h"
#include "../OpenGLUtils/include/EGLThread.h"
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <android/asset_manager_jni.h>
extern "C" {
#include "../ffmpeg/include/libavutil/avutil.h"
}
namespace TranglesNF{
EGLThread *eglThread = NULL;
Trangles mShape;

void callBackOnCreate() {
    mShape.initGL(mShape.verticesShaderSource, mShape.FRAGMENT_SHADER);
}

void callBackOnChange(int width, int height) {
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    LOGI("I come here");
    mShape.draw();
}

void callBackOnDraw() {
    mShape.draw();
}

}


extern "C"{
JNIEXPORT void JNICALL
Java_com_zxx_camera_renderer_NativeTrangleRender_nativeInit(JNIEnv *env, jclass clazz, jobject  surface,jstring vertexShaderCode_,jstring fragmentShaderCode_){
    const char *vertexShaderCode = env->GetStringUTFChars(vertexShaderCode_, 0);
    const char *fragmentShaderCode = env->GetStringUTFChars(fragmentShaderCode_, 0);

    basic::Thread *thread = new basic::Thread();
    std::function<void()> function = [](){
        LOGD("run task");
    };
    if(thread->start()){
        thread->runTask(function);
        thread->runTask(function);
        thread->runTask(function);
    } else{
        LOGD("thread create error");
    }



    TranglesNF::eglThread = new EGLThread();
    TranglesNF::eglThread->setonCreateCallback(TranglesNF::callBackOnCreate);
    TranglesNF::eglThread->setonChangeCallback(TranglesNF::callBackOnChange);
    TranglesNF::eglThread->setonDrawCallback(TranglesNF::callBackOnDraw);

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    TranglesNF::eglThread->onSurfaceCreate(nativeWindow);

    env->ReleaseStringUTFChars(vertexShaderCode_, vertexShaderCode);
    env->ReleaseStringUTFChars(fragmentShaderCode_, fragmentShaderCode);
}
JNIEXPORT void JNICALL
Java_com_zxx_camera_renderer_NativeTrangleRender_nativeSurfaceChanged(JNIEnv *env, jclass clazz,jint width, jint height){
    if (TranglesNF::eglThread) {
        TranglesNF::eglThread->onSurfaceChange(width, height);
    }



}
JNIEXPORT void JNICALL//nativeSurfaceDestroyed
Java_com_zxx_camera_renderer_NativeTrangleRender_nativeDraw(JNIEnv *env, jclass clazz){
    if(TranglesNF::eglThread != nullptr){

    }
}
JNIEXPORT void JNICALL
Java_com_zxx_camera_renderer_NativeTrangleRender_nativeStopEGLThread(JNIEnv *env, jclass clazz){
    TranglesNF::eglThread->onSurfaceDestroyed();
}
}
EGLThread *eglThread = nullptr;
DrawerOES *mDrawOES = nullptr;

void callBackOnCreate(){
    mDrawOES = DrawerOES::getInstance();
    //mDrawOES->openCamera(eglThread->surfaceWidth,eglThread->surfaceHeight);
    mDrawOES->create();
}

void callBackOnChange(int width, int height){
    glViewport(0,0,width,height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    LOGI("I come here");
    mDrawOES = DrawerOES::getInstance();
    mDrawOES->update();
    mDrawOES->draw(mDrawOES->texture);
}

void callBackOnDraw(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    mDrawOES = DrawerOES::getInstance();
    mDrawOES->update();
    mDrawOES->draw(mDrawOES->texture);
}

extern "C"{
JNIEXPORT void JNICALL
Java_com_zxx_camera_renderer_NativeCameraRender_registerAssetManager(JNIEnv *env, jclass thiz,
        jobject asset_manager) {
    if (asset_manager) {
        DrawerOES::getInstance()->g_pAssetManager = AAssetManager_fromJava(env, asset_manager);
    } else {
        LOGE("assetManager is null!")
    }
}

/*----------------------------------------------------------------------------------------------------
* native camera
----------------------------------------------------------------------------------------------------*/
JNIEXPORT void JNICALL
Java_com_zxx_camera_renderer_NativeCameraRender_nativeInit(JNIEnv *env, jclass clazz, jobject  surface,jobject mCameraProxy,jint width,jint height){
    mDrawOES = DrawerOES::getInstance();

    env->GetJavaVM(&mDrawOES->global_jvm);
    eglThread = new EGLThread();
    eglThread->setonCreateCallback(callBackOnCreate);
    eglThread->setonChangeCallback(callBackOnChange);
    eglThread->setonDrawCallback(callBackOnDraw);

    jclass obj = env->GetObjectClass(mCameraProxy);
    mDrawOES->mCameraProxy =env->NewGlobalRef(mCameraProxy);

    mDrawOES->openCameraMethodId = (*env).GetMethodID(obj,"openCamera", "(II)V");

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    eglThread->setWidthandHeight(width,height);
    eglThread->onSurfaceCreate(nativeWindow);
    env->DeleteLocalRef( obj );
}
JNIEXPORT void JNICALL
Java_com_zxx_camera_renderer_NativeCameraRender_surfaceChanged(JNIEnv *env, jclass clazz,jint width, jint height){
    if (eglThread) {
        eglThread->onSurfaceChange(width, height);
    }
}
JNIEXPORT jobject JNICALL
        Java_com_zxx_camera_renderer_NativeCameraRender_nativeGetSurfaceTexture(JNIEnv *env, jclass clazz){
     DrawerOES::getInstance()->setupSurfaceTexture(env);
    return  DrawerOES::getInstance()->javaSurfaceTextureObj;
}
JNIEXPORT void JNICALL
        Java_com_zxx_camera_renderer_NativeCameraRender_nativeStopEGLThread(JNIEnv *env, jclass clazz){
    eglThread->onSurfaceDestroyed();
}
}
