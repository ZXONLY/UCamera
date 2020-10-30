//
// Created by bytedance on 30/10/2020.
//

#ifndef CAMERA_JAVALISTENER_H
#define CAMERA_JAVALISTENER_H

#include <jni.h>

class JavaListener {
//定义类成员属性
public:
    JavaVM *vm;
    JNIEnv *env;
    jobject jobj;
    jmethodID jmethod;


public:
    //定义构造函数
    JavaListener(JavaVM *vm, JNIEnv *env, jobject jobj);

    //析构函数
    ~JavaListener();
    //定义类成员方法 onSuccess 在这个函数会负责去调用 Java 层的 onSuccess 函数
    void onSuccess(const char *msg);
};
#endif //CAMERA_JAVALISTENER_H
