//
// Created by bytedance on 27/10/2020.
//

#ifndef CAMERA_TRANGLES_H
#define CAMERA_TRANGLES_H

#include "LogUtils.h"
#include "../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/GLES3/gl3.h"

class Trangles{
public:
    GLint mProgram;
    GLint mAPositionHandle;
    GLint mAColorHandle;
    GLfloat *mVertexArray;
    GLfloat *mColorArray;

    Trangles();
    virtual ~Trangles();
    void initVertex();
    void initGL(const char *vertexShaderCode, const char *fragmentShaderCode);
    void draw();

};



#endif //CAMERA_TRANGLES_H;
