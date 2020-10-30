////
//// Created by bytedance on 27/10/2020.
////
//
#include <jni.h>
#include "Trangles.h"
#include "EGLHelper.h"
#include "EGLThread.h"
#include "DrawerOES.h"
#include "../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/android/native_window_jni.h"
#include "../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/android/native_window.h"

//EGLThread *eglThread = NULL;
//Trangles mShape;
//
//void callBackOnCreate(){
//    mShape.initGL(mShape.verticesShaderSource, mShape.FRAGMENT_SHADER);
//}
//
//void callBackOnChange(int width, int height){
//    glViewport(0,0,width,height);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
//    LOGI("I come here");
//    mShape.draw();
//}
//
//void callBackOnDraw(){
//    mShape.draw();
//}
//
//
//extern "C"{
//JNIEXPORT void JNICALL
//Java_com_zxx_camera_renderer_NativeTrangleRender_nativeInit(JNIEnv *env, jclass clazz, jobject  surface,jstring vertexShaderCode_,jstring fragmentShaderCode_){
//    const char *vertexShaderCode = env->GetStringUTFChars(vertexShaderCode_, 0);
//    const char *fragmentShaderCode = env->GetStringUTFChars(fragmentShaderCode_, 0);
//
//    eglThread = new EGLThread();
//    eglThread->setonCreateCallback(callBackOnCreate);
//    eglThread->setonChangeCallback(callBackOnChange);
//    eglThread->setonDrawCallback(callBackOnDraw);
//
//    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
//    eglThread->onSurfaceCreate(nativeWindow);
//
//    env->ReleaseStringUTFChars(vertexShaderCode_, vertexShaderCode);
//    env->ReleaseStringUTFChars(fragmentShaderCode_, fragmentShaderCode);
//}
//JNIEXPORT void JNICALL
//Java_com_zxx_camera_renderer_NativeTrangleRender_nativeSurfaceChanged(JNIEnv *env, jclass clazz,jint width, jint height){
//    if (eglThread) {
//        eglThread->onSurfaceChange(width, height);
//    }
//
//
//
//}
//JNIEXPORT void JNICALL//nativeSurfaceDestroyed
//Java_com_zxx_camera_renderer_NativeTrangleRender_nativeDraw(JNIEnv *env, jclass clazz){
//    if(eglThread != nullptr){
//
//    }
//}
//}
EGLThread *eglThread = nullptr;
DrawerOES *mDrawOES = nullptr;

void callBackOnCreate(){
    mDrawOES = DrawerOES::getInstance();
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
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
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
Java_com_zxx_camera_renderer_NativeCameraRender_nativeInit(JNIEnv *env, jclass clazz, jobject  surface){
    mDrawOES = DrawerOES::getInstance();
    env->GetJavaVM(&mDrawOES->global_jvm);
    eglThread = new EGLThread();
    eglThread->setonCreateCallback(callBackOnCreate);
    eglThread->setonChangeCallback(callBackOnChange);
    eglThread->setonDrawCallback(callBackOnDraw);

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    eglThread->onSurfaceCreate(nativeWindow);
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

}
