////
//// Created by bytedance on 29/10/2020.
////
//#include <jni.h>
//#include "EGLHelper.h"
//#include "EGLThread.h"
//#include "DrawerOES.h"
//#include <android/native_window_jni.h>
//#include <android/native_window.h>
//
//
//EGLThread *eglThread = nullptr;
//DrawerOES *mDrawOES = nullptr;
//
//void callBackOnCreate(){
//    mDrawOES = DrawerOES::getInstance();
//    mDrawOES->create();
//}
//
//void callBackOnChange(int width, int height){
//    glViewport(0,0,width,height);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
//    LOGI("I come here");
//    mDrawOES = DrawerOES::getInstance();
//    mDrawOES->draw(mDrawOES->texture);
//}
//
//void callBackOnDraw(){
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
//    LOGI("I come here");
//    mDrawOES = DrawerOES::getInstance();
//    mDrawOES->draw(mDrawOES->texture);
//}
//
//extern "C"{
//JNIEXPORT void JNICALL
//Java_com_zxx_camera_renderer_NativeCameraRender_registerAssetManager(JNIEnv *env, jclass thiz,
//                                                                         jobject asset_manager) {
//    if (asset_manager) {
//        DrawerOES::getInstance()->g_pAssetManager = AAssetManager_fromJava(env, asset_manager);
//    } else {
//        LOGE("assetManager is null!")
//    }
//}
//
///*----------------------------------------------------------------------------------------------------
//* native camera
//----------------------------------------------------------------------------------------------------*/
//JNIEXPORT void JNICALL
//Java_com_zxx_camera_renderer_NativeCameraRender_nativeInit(JNIEnv *env, jclass clazz, jobject  surface,jint texture){
//    eglThread = new EGLThread();
//    eglThread->setonCreateCallback(callBackOnCreate);
//    eglThread->setonChangeCallback(callBackOnChange);
//    eglThread->setonDrawCallback(callBackOnDraw);
//    mDrawOES = DrawerOES::getInstance();
//    mDrawOES->texture = texture;
//    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
//    eglThread->onSurfaceCreate(nativeWindow);
//}
//JNIEXPORT void JNICALL
//        Java_com_zxx_camera_renderer_NativeCameraRender_surfaceChanged(JNIEnv *env, jclass clazz,jint width, jint height){
//    if (eglThread) {
//        eglThread->onSurfaceChange(width, height);
//    }
//}
//}