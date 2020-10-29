//
// Created by bytedance on 27/10/2020.
//

#ifndef CAMERA_GLUTIL_H
#define CAMERA_GLUTIL_H

#include "../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/GLES3/gl3.h"
#include "LogUtils.h"

    static int complieShader(int type, const char* shaderCode);
    static int createProgram(const char* vertexShaderCode, const char* fragmentShaderCode);

#endif //CAMERA_GLUTIL_H
