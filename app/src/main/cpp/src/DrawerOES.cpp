#include <DrawerOES.h>
#include <GLES2/gl2ext.h>
#include "Mat.h"
//
// Created by bytedance on 29/10/2020.
//

const GLfloat squareCoords[8] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f,  1.0f
};

const GLfloat textureVertices[8] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
};
const GLfloat mMatrix[4][4] = {
//            1.3666667,0.0,0.0,0.0,
//            0.0, 1.0, 0.0, 0.0,
//            0.0, 0.0, -1.0, 0.0,
//            0.0, 0.0, -1.0, 1.0
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
};
//const GLbyte VERTEX_ORDER[] = { 0, 1, 2, 3 }; // order to draw vertices
DrawerOES *DrawerOES::mDrawer = nullptr;
void DrawerOES::create() {
    char *vertexShaderSource = GLUtil::readAssetFile("vertexOES.vsh",g_pAssetManager);
    LOGI("%s",vertexShaderSource);
    char *fragmentShaderSource = GLUtil::readAssetFile("fragmentOES.fsh", g_pAssetManager);
    LOGD("%s",fragmentShaderSource);
    mProgram = GLUtil::createProgram(vertexShaderSource,fragmentShaderSource);
    if(mProgram == GL_NONE){
        LOGE("gl init failed!");
    }

    glClearColor(0.0f,0.0f,1.0f,1.0f);
}
DrawerOES* DrawerOES::getInstance() {
    if(DrawerOES::mDrawer == nullptr) DrawerOES::mDrawer = new DrawerOES();
    return DrawerOES::mDrawer;
}
DrawerOES::DrawerOES() {
}
DrawerOES::~DrawerOES() {
    glDisableVertexAttribArray(g_position_handle);
    glDisableVertexAttribArray(g_matrix_handler);
    glDisableVertexAttribArray(g_texture_handler);
    glDisableVertexAttribArray(g_texture_coord_handler);
    glDeleteProgram(mProgram);
}

void DrawerOES::draw(GLuint texture) {
    glClear(GL_COLOR_BUFFER_BIT);
// 1. 选择使用的程序
    glUseProgram(mProgram);
    g_position_handle = glGetAttribLocation(mProgram,"vPosition");
    g_texture_handler = glGetUniformLocation(mProgram,"yuvTexSampler");
    g_texture_coord_handler = glGetAttribLocation(mProgram,"aTextureCoord");
    g_matrix_handler = glGetUniformLocation(mProgram,"uMVPMatrix");
    //update();
//2.绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,texture);
//3.加载顶点数据 glVertexAttribPointer
    glVertexAttribPointer(g_position_handle,2,GL_FLOAT,false,8,squareCoords);
    glEnableVertexAttribArray(g_position_handle);


    glVertexAttribPointer(g_texture_coord_handler,2,GL_FLOAT, false,8,textureVertices);
    glEnableVertexAttribArray(g_texture_coord_handler);

////    Mat::matrixSetRotateM(mMatrix,180,0.0f,1.0f,0.0f);
////    Mat::matrixRotateM(mMatrix, -90, 0.0f, 0.0f, 1.0f);
//    for(int i = 0;i<4;i++){
//        LOGD("%f %f %f %f",mMatrix[i*4+0],mMatrix[i*4+1],mMatrix[i*4+2],mMatrix[i*4+3])
//    }
    glUniformMatrix4fv(g_matrix_handler, 1, GL_FALSE, &mMatrix[0][0]);
    glUniform1i(g_texture_handler,0);
    //glDrawElements(GL_TRIANGLE_FAN,4,GL_BYTE,VERTEX_ORDER);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(g_position_handle);
    glDisableVertexAttribArray(g_texture_coord_handler);
}

void DrawerOES::setupSurfaceTexture(JNIEnv *jni) {
    glGenTextures(1,&texture);

    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const char *stClassPath = "android/graphics/SurfaceTexture";
    const jclass surfaceTextureClass = jni->FindClass(stClassPath);
    if (surfaceTextureClass == 0) {
        LOGE("FindClass (%s) failed", stClassPath);
    }
    const jmethodID constructor = jni->GetMethodID( surfaceTextureClass, "<init>", "(I)V" );
    if (constructor == 0) {
        LOGE("GetMethonID(<init>) failed");
    }
    int tex = texture;
    LOGI("%d %d",tex,texture);
    jobject obj = jni->NewObject(surfaceTextureClass, constructor,tex);
    if (obj == 0) {
        LOGE("NewObject() failed");
    }

    javaSurfaceTextureObj = jni->NewGlobalRef(obj);
    if (javaSurfaceTextureObj == 0) {
        LOGE("NewGlobalRef() failed");
    }
    //Now that we have a globalRef, we can free the localRef
    jni->DeleteLocalRef(obj);
    updateTexImageMethodId = jni->GetMethodID( surfaceTextureClass, "updateTexImage", "()V");
    if ( !updateTexImageMethodId ) {
        LOGE("couldn't get updateTexImageMethonId");
    }
    jni->DeleteLocalRef( surfaceTextureClass );
}
JNIEnv* DrawerOES::get_env() {
    if (global_jvm == NULL) return NULL;

    JNIEnv *jni_env = NULL;

    int status = global_jvm->GetEnv((void **)&jni_env, JNI_VERSION_1_6);

    if (status == JNI_EDETACHED || jni_env == NULL) {
        status = global_jvm->AttachCurrentThread(&jni_env, NULL);
        if (status < 0) {
            jni_env = NULL;
        }
    }
    return jni_env;
}
void DrawerOES::update() {
    if(!javaSurfaceTextureObj){
        return;
    }
    JNIEnv *jniEnv =get_env();
    if(jniEnv!=nullptr){
        LOGI("can get JNIENV");
        jniEnv->CallVoidMethod(javaSurfaceTextureObj, updateTexImageMethodId);
    } else{
        LOGE("can't get JNIENV");
    }
}
void DrawerOES::openCamera(int width,int height) {
    if(!mCameraProxy){
        return;
    }
    JNIEnv *jniEnv =get_env();
    if(jniEnv!=nullptr){
        LOGI("open camera");
        jniEnv->CallVoidMethod(mCameraProxy,openCameraMethodId,width,height);
    } else{
        LOGE("can't open camera");
    }
}
