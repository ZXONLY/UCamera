//
// Created by bytedance on 27/10/2020.
//

#include <jni.h>
#include "Trangles.h"

Trangles mShape;

extern "C"{
JNIEXPORT void JNICALL
Java_com_zxx_camera_renderer_NativeTrangleRender_nativeInit(JNIEnv *env, jclass clazz, jstring vertexShaderCode_,jstring fragmentShaderCode_){
    const char *vertexShaderCode = env->GetStringUTFChars(vertexShaderCode_, 0);
    const char *fragmentShaderCode = env->GetStringUTFChars(fragmentShaderCode_, 0);

    // TODO

    mShape.initGL(vertexShaderCode, fragmentShaderCode);

    env->ReleaseStringUTFChars(vertexShaderCode_, vertexShaderCode);
    env->ReleaseStringUTFChars(fragmentShaderCode_, fragmentShaderCode);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glDisable(GL_DEPTH_TEST);
}
JNIEXPORT void JNICALL
        Java_com_zxx_camera_renderer_NativeTrangleRender_nativeDraw(JNIEnv *env, jclass clazz){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mShape.draw();
}

}
