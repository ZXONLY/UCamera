

#include "include/UAndroidJni.h"
#include <unistd.h>
#include <pthread.h>
#include <jni.h>
#include "include/UJClassBase.h"
#include "LogUtils.h"
#undef TE_LOG_TAG
#define TE_LOG_TAG "CAndroidJni"

extern "C" {

static JavaVM *gJvm = NULL;
static pthread_key_t mThreadKey = 0;

static void Android_JNI_ThreadDestroyed(void *value) {
    LOGI("pthread JNI detach current thread %p", value);
    JNIEnv *env = (JNIEnv *)value;
    if (env != NULL) {
        gJvm->DetachCurrentThread();
        pthread_setspecific(mThreadKey, NULL);
    }
}

JavaVM *U_JNI_GetJvm() {
    return gJvm;
}

bool U_JNI_GetJNIEnv(JNIEnv *&pEnv) {
    if (gJvm == nullptr) {
        LOGE("%s: JVM is null get JniEnv failed", __FUNCTION__);
        pEnv = nullptr;
        return false;
    }
    int status = gJvm->GetEnv((void **)&pEnv, JNI_VERSION_1_6);
    if (status == JNI_EDETACHED) {
        if (gJvm->AttachCurrentThread(&pEnv, NULL) != JNI_OK) {
            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
            return false;
        }
        LOGI("pthread JNI attach current thread %p", (void *)pEnv);
        pthread_setspecific(mThreadKey, (void *)pEnv);

        return true;
    } else if (status == JNI_OK) {
        return true;
    } else {
        LOGE("%s: AttachCurrentThread() failed with error=%d", __FUNCTION__, status);
        return false;
    }
}

int U_JNI_ThrowException(JNIEnv *env, const char *className, const char *msg) {
    if (env->ExceptionCheck()) {
        jthrowable exception = env->ExceptionOccurred();
        env->ExceptionClear();

        if (exception != NULL) {
            LOGD("Discarding pending exception (%s) to throw", className);
            env->DeleteLocalRef(exception);
        }
    }

    jclass exceptionClass = env->FindClass(className);
    if (exceptionClass == NULL) {
        LOGD("Unable to find exception class %s", className);
        /* ClassNotFoundException now pending */
        goto fail;
    }

    if (env->ThrowNew(exceptionClass, msg) != JNI_OK) {
        LOGD("Failed throwing '%s' '%s'", className, msg);
        /* an exception, most likely OOM, will now be pending */
        goto fail;
    }

    return 0;
    fail:
    if (exceptionClass)
        env->DeleteLocalRef(exceptionClass);
    return -1;
}

int U_JNI_ThrowIllegalStateException(JNIEnv *env, const char *msg) {
    return U_JNI_ThrowException(env, "java/lang/IllegalStateException", msg);
}

void U_JNI_DeleteGlobalRefP(JNIEnv *env, jobject *obj_ptr) {
    if (!obj_ptr || !*obj_ptr)
        return;

    env->DeleteGlobalRef(*obj_ptr);
    *obj_ptr = NULL;
}

void U_JNI_DeleteGlobalRefP1(jobject *obj_ptr) {
    if (!obj_ptr || !*obj_ptr)
        return;
    JNIEnv *env;
    U_JNI_GetJNIEnv(env);
    env->DeleteGlobalRef(*obj_ptr);
    *obj_ptr = NULL;
}

void U_JNI_DeleteLocalRefP(JNIEnv *env, jobject *obj_ptr) {
    if (!obj_ptr || !*obj_ptr)
        return;

    env->DeleteLocalRef(*obj_ptr);
    *obj_ptr = NULL;
}

bool U_ExceptionCheck__throwAny(JNIEnv *env) {
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        return true;
    }

    return false;
}

bool U_ExceptionCheck__catchAll(JNIEnv *env) {
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return true;
    }

    return false;
}

static jobject gClassLoader = nullptr;
static jmethodID gFindClassMethod = nullptr;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGE("====== JNI_OnLoad ======");

    gJvm = vm;

    if (pthread_key_create(&mThreadKey, Android_JNI_ThreadDestroyed) != JNI_OK) {
        LOGE("Error initializing pthread key");
    }

    JNIEnv *pEnv = nullptr;
    vm->GetEnv((void **)&pEnv, JNI_VERSION_1_6);
//    jclass btchProcessInterfaceClass = pEnv->FindClass("com/benchmark/ByteBenchProcessInterface");
//    btch::CJClassBase::registerClass("com/benchmark/ByteBenchProcessInterface", pEnv, btchProcessInterfaceClass);
//
//    jclass monitorWrapperClass = pEnv->FindClass("com/benchmark/monitor/CBatteryMonitor");
//    btch::CJClassBase::registerClass("com/benchmark/monitor/CBatteryMonitor", pEnv, monitorWrapperClass);
//
//    jclass btchServiceHealthClass = pEnv->FindClass("com/benchmark/ByteBenchServiceHealth");
//    btch::CJClassBase::registerClass("com/benchmark/ByteBenchServiceHealth", pEnv, btchServiceHealthClass);
//
//    jclass hwdecodeClass = pEnv->FindClass("com/benchmark/presenter/HWDecodeBridge");
//    btch::CJClassBase::registerClass("com/benchmark/presenter/HWDecodeBridge", pEnv, hwdecodeClass);
//
//    jclass hwdecodeStClass = pEnv->FindClass("com/benchmark/settings/BTCDecodeSettings");
//    btch::CJClassBase::registerClass("com/benchmark/settings/BTCDecodeSettings", pEnv, hwdecodeStClass);
//
//    jclass hwencodeClass = pEnv->FindClass("com/benchmark/presenter/HWEncodeManager");
//    btch::CJClassBase::registerClass("com/benchmark/presenter/HWEncodeManager", pEnv, hwencodeClass);
//
//    jclass hwencodeStClass = pEnv->FindClass("com/benchmark/settings/BTCEncodeSettings");
//    btch::CJClassBase::registerClass("com/benchmark/settings/BTCEncodeSettings", pEnv, hwencodeStClass);
//
//    jclass hwencodeSettingsClass = pEnv->FindClass("com/benchmark/mediacodec/TEMediaCodecEncodeSettings");
//    btch::CJClassBase::registerClass("com/benchmark/mediacodec/TEMediaCodecEncodeSettings", pEnv, hwencodeSettingsClass);
//
//    jclass ioClass = pEnv->FindClass("com/benchmark/presenter/IOBridge");
//    btch::CJClassBase::registerClass("com/benchmark/presenter/IOBridge", pEnv, ioClass);
//
//    jclass ioStClass = pEnv->FindClass("com/benchmark/settings/BTCIOSettings");
//    btch::CJClassBase::registerClass("com/benchmark/settings/BTCIOSettings", pEnv, ioStClass);

    return JNI_VERSION_1_6;
}

JNIEXPORT jclass U_FindClass(JNIEnv *env, const char *name) {
    jclass result = nullptr;
    if (env) {
        //这句会出错，所以要处理错误
        result = env->FindClass(name);
        jthrowable exception = env->ExceptionOccurred();
        if (exception) {
            env->ExceptionClear();
            if (gClassLoader != nullptr && gFindClassMethod != nullptr) {
                jstring pJstring = env->NewStringUTF(name);
                jclass ret = static_cast<jclass>(env->CallObjectMethod(gClassLoader, gFindClassMethod, pJstring));
                env->DeleteLocalRef(pJstring);
                if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                    LOGE("Class not found %s ret=%p", name, (void *)ret);
                    return nullptr;
                }
                return ret;
            } else {
                return nullptr;
            }
        }
    }
    return result;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
    pthread_key_delete(mThreadKey);
}
}
