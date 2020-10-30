//
// Created by bytedance on 30/10/2020.
//
#include "JavaListener.h"
JavaListener::JavaListener(JavaVM *vm, JNIEnv *env, jobject jobj) {
    this->vm = vm;
    this->env = env;
    this->jobj = jobj;

    jclass jclaz = env->GetObjectClass(jobj);

    //得到 jmethodid 实例
    jmethod = env->GetMethodID(jclaz, "onSuccess", "(Ljava/lang/String;)V");

}

//在子线程回调这个方法 onSuccess
void JavaListener::onSuccess(const char *msg) {

    //得到子线程相关的 JNIEnv 实例
    JNIEnv *env;
    vm->AttachCurrentThread(&env, 0);

    //将需要传递给 Java 层 onSuccess 的 msg 转化为 jstring 实例
    jstring jmsg = env->NewStringUTF(msg);

    //调用 Java 层的函数
    env->CallVoidMethod(jobj, jmethod, jmsg);
    //删除本地引用 jmsg，避免内存泄露
    env->DeleteLocalRef(jmsg);
    //取消挂载线程
    vm->DetachCurrentThread();
}
