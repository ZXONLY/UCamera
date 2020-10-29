//
// Created by bytedance on 27/10/2020.
//

#include "../inc/GLUtil.h"
#include "../../../../../../../Library/Android/sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/GLES3/gl3.h"
#include <stdlib.h>

int complieShader(int type, const char *shaderCode) {
    int shader = glCreateShader(type);
    if(shader==0){}
    glShaderSource(shader,1,&shaderCode, nullptr);
    glCompileShader(shader);
    GLint compileStatus = 0;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&compileStatus);
    if(!compileStatus){
        glDeleteShader(shader);
        LOGE("compile shader error");
        return 0;
    }
    return shader;
}

int createProgram(const char *vertexShaderCode, const char *fragmentShaderCode) {
    GLint program = glCreateProgram();
    if(0==program){
        LOGE("create program error");
    }
    LOGI("create program sucess");
    int vertexShader = complieShader(GL_VERTEX_SHADER,vertexShaderCode);
    int fragmentShader = complieShader(GL_FRAGMENT_SHADER,fragmentShaderCode);

    glAttachShader(program,vertexShader);
    glAttachShader(program,fragmentShader);
    glLinkProgram(program);
    GLint linkStatus;
    glGetProgramiv(program,GL_LINK_STATUS,&linkStatus);
    if(0 == linkStatus){
        glDeleteProgram(program);
        LOGE("link program error");
        return 0;
    }
    return program;
}

char *readAssetFile(const char *filename, AAsetManager *mgr)