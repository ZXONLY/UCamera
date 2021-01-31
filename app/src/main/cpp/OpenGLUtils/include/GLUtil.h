//
// Created by bytedance on 27/10/2020.
//

#ifndef CAMERA_GLUTIL_H
#define CAMERA_GLUTIL_H

#include "../../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/GLES3/gl3.h"
#include "../../inc/LogUtils.h"
#include "../../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/android/asset_manager_jni.h"
class GLUtil {
public:
    static int complieShader(int type, const char *shaderCode);

    static char *readAssetFile(const char *filename, AAssetManager *);

    static int createProgram(const char *vertexShaderCode, const char *fragmentShaderCode);
};
#endif //CAMERA_GLUTIL_H
