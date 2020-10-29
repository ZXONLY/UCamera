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

    const char *verticesShaderSource ="#version 300 es\nlayout (location = 0) in vec4 aPosition;\nlayout (location = 1) in vec4 aColor;\nout vec4 vColor;\nvoid main() {\n gl_Position  = aPosition;\nvColor = aColor;\n}";

    const char *FRAGMENT_SHADER ="#version 300 es\nprecision mediump float;\nin vec4 vColor;\nout vec4 fragColor;\nvoid main() {\n     fragColor = vColor;\n}";

    Trangles();
    virtual ~Trangles();
    void initVertex();
    void initGL(const char *vertexShaderCode, const char *fragmentShaderCode);
    void draw();

};



#endif //CAMERA_TRANGLES_H;
