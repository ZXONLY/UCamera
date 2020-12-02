//
// Created by bytedance on 27/10/2020.
//
#include "Trangles.h"
#include "GLUtil.h"

const GLfloat mMatrix[4][4] = {
            1.3666667,0.0,0.0,0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, -1.0, 0.0,
            0.0, 0.0, -1.0, 1.0
        //        1.0f, 0.0f, 0.0f, 0.0f,
//        0.0f, 1.0f, 0.0f, 0.0f,
//        0.0f, 0.0f, 1.0f, 0.0f,
//        0.0f, 0.0f, 0.0f, 1.0f
};

void Trangles::initGL(const char *vertexShaderCode, const char *fragmentShaderCode) {
    mProgram = GLUtil::createProgram(vertexShaderCode,fragmentShaderCode);
    mAPositionHandle = glGetAttribLocation(mProgram,"aPosition");
    mAColorHandle = glGetAttribLocation(mProgram,"aColor");
    mUMVPMatrixHandle = glGetUniformLocation(mProgram,"uMVPMatrix");
}
void Trangles::initVertex() {
    mVertexArray = new GLfloat[3*4];
    mColorArray = new GLfloat[3*4];

    mVertexArray[0]=0;mVertexArray[1]=0.5;mVertexArray[2]=0;mVertexArray[3]=1;
    mVertexArray[4]=-0.5;mVertexArray[5]=-0.5;mVertexArray[6]=0;mVertexArray[7]=1;
    mVertexArray[8]=0.5;mVertexArray[9]=-0.5;mVertexArray[10]=0;mVertexArray[11]=1;

    mColorArray[0]=0;mColorArray[1]=1;mColorArray[2]=0;mColorArray[3]=1;
    mColorArray[4]=1;mColorArray[5]=0;mColorArray[6]=0;mColorArray[7]=1;
    mColorArray[8]=0;mColorArray[9]=0;mColorArray[10]=1;mColorArray[11]=1;
}

void Trangles::draw() {
    glUseProgram(mProgram);
    glClearColor(1.0f,1.0f,1.0f,0.0f);
    glVertexAttribPointer(mAPositionHandle,4,GL_FLOAT, false,4*4,mVertexArray);

    glVertexAttribPointer(mAColorHandle,4,GL_FLOAT, false,4*4,mColorArray);

    glUniformMatrix4fv(mUMVPMatrixHandle,1, false,&mMatrix[0][0]);

    glEnableVertexAttribArray(mAPositionHandle);
    glEnableVertexAttribArray(mAColorHandle);
    glDrawArrays(GL_TRIANGLE_FAN,0,3);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
}

Trangles::Trangles() {
    initVertex();
}

Trangles::~Trangles() {
    delete []mVertexArray;
    delete []mColorArray;
}