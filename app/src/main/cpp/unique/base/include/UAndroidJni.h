//
// Created by bytedance on 10/03/2021.
//
#pragma once

#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>

JavaVM *U_JNI_GetJvm();

bool U_JNI_GetJNIEnv(JNIEnv *&pEnv);

bool U_ExceptionCheck__catchAll(JNIEnv *env);

bool U_ExceptionCheck__throwAny(JNIEnv *env);

int U_JNI_ThrowException(JNIEnv *env, const char *exception, const char *msg);
int U_JNI_ThrowIllegalStateException(JNIEnv *env, const char *msg);

jobject U_JNI_NewObjectAsGlobalRef(JNIEnv *env, jclass clazz, jmethodID methodID, ...);

void U_JNI_DeleteGlobalRefP(JNIEnv *env, jobject *obj_ptr);
void U_JNI_DeleteGlobalRefP1(jobject *obj_ptr);
void U_JNI_DeleteLocalRefP(JNIEnv *env, jobject *obj_ptr);

JNIEXPORT jclass U_FindClass(JNIEnv *env, const char *name);


#ifdef __cplusplus
}
#endif

