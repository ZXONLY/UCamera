//
// Created by bytedance on 10/03/2021.
//

#include "include/UJClassBase.h"

#include "LogUtils.h"
#include "UResult.h"
#include "UAndroidJni.h"

#ifdef TAG
#undef TAG
#endif

#define TAG "TEJClassBase"

std::map<std::string, jclass > UJClassBase::s_classMap;

UJClassBase::UJClassBase(const std::string &strJClassName)
        : m_strJClassName(strJClassName), m_isInit(false) {
}

UJClassBase::~UJClassBase() {
    release();
}

int UJClassBase::init() {
    JNIEnv* env;
    U_JNI_GetJNIEnv(env);
    //Attention: You can not call "FindClass" to get the "jclass", this is dengerous operation!
    //Find class during JNI_OnLoad, and keep it in s_classMap.
    jclass jclazz = s_classMap[m_strJClassName];
    if (jclazz == nullptr) {
        LOGE("Find [%s] failed.", m_strJClassName.c_str());
        return UR_JNI_ERROR;
    }
    UResult ret = _initMethodIDs(jclazz);
    if (ret != UR_OK) {
        LOGE("_initMethodIDs failed");
        return ret;
    }

    ret = _createObject(jclazz);
    if (ret != UR_OK) {
        LOGE("_createObject failed");
        return ret;
    }

    m_isInit = true;
    return ret;
}

int UJClassBase::init(JNIEnv *env, jobject jobj) {

    if (env == nullptr || jobj == nullptr) {
        LOGE("Invalid params!");
        return UR_INVALID_PARAM;
    }

    mJObj = env->NewGlobalRef(jobj);

    jclass jclazz = env->GetObjectClass(jobj);
    if (jclazz == nullptr)
    {
        LOGE("Class not found.");
        return UR_FAIL;
    }

    UResult ret = _initMethodIDs(jclazz);
    m_isInit = true;
    return ret;
}

void UJClassBase::release() {
    LOGD("TEJClassBase::release.");
    if (!m_isInit) {
        return;
    }
    JNIEnv* env;
    U_JNI_GetJNIEnv(env);
    if(mJObj!= nullptr) {
        env->DeleteGlobalRef(mJObj);
    }
    m_isInit = false;
}

void UJClassBase::registerClass(const std::string &strClassName, JNIEnv* pEnv, jclass &jclazz) {
    jclass globalClass = reinterpret_cast<jclass>(pEnv->NewGlobalRef(jclazz));
    s_classMap.emplace(strClassName, globalClass);
}
jclass UJClassBase::getClass(const std::string &strClassName) {
    return s_classMap[strClassName];
}