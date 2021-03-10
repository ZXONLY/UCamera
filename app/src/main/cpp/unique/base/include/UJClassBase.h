//
// Created by bytedance on 10/03/2021.
//
#pragma once


#include <jni.h>
#include <string>
#include <map>
#include <atomic>

#include "UAndroidJni.h"
#include "UResult.h"

class UJClassBase {
public:
    UJClassBase(const std::string& strJClassName);
    virtual ~UJClassBase();

    virtual UResult init();

    virtual UResult init(JNIEnv* env, jobject jobj);

    virtual void release();

//    template <typename Func, typename Arg>
//    auto jniFuncWrapper(Func func, Arg arg) -> decltype(func(arg))
//    {
//        return func(arg);
//    };

    static void registerClass(const std::string& strClassName, JNIEnv* pEnv, jclass& jclazz);
    static jclass getClass(const std::string &strClassName);

protected:

    virtual UResult _initMethodIDs(jclass jclazz) = 0;
    virtual UResult _createObject(jclass jclazz) = 0;

protected:
    std::string m_strJClassName;
    jobject mJObj = nullptr;
    std::atomic<bool> m_isInit;

    static std::map<std::string, jclass > s_classMap;
};
