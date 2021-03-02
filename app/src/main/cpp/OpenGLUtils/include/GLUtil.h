//
// Created by bytedance on 27/10/2020.
//

#ifndef CAMERA_GLUTIL_H
#define CAMERA_GLUTIL_H

#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include "../../inc/LogUtils.h"

class GLUtil {
public:
    static int complieShader(int type, const char *shaderCode);

    static char *readAssetFile(const char *filename, AAssetManager *);

    static int createProgram(const char *vertexShaderCode, const char *fragmentShaderCode);
};
#endif //CAMERA_GLUTIL_H
