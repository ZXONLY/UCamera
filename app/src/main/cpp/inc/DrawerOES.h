//
// Created by bytedance on 29/10/2020.
//

#ifndef CAMERA_DRAWEROES_H
#define CAMERA_DRAWEROES_H

#include <android/asset_manager.h>
#include <jni.h>
#include <GLES3/gl3.h>
#include "../OpenGLUtils/GLUtil.h"

class DrawerOES{
public:
    void create();
    void draw(GLuint texture);
    void draw(GLuint texture, float* mMatrix);
    static DrawerOES* getInstance();
    void setupSurfaceTexture(JNIEnv *jni);
    void update();
    void openCamera(int width,int height);
    AAssetManager *g_pAssetManager;
    ~DrawerOES();
    GLuint texture;
    jobject javaSurfaceTextureObj;
    JavaVM *global_jvm;
    jobject mCameraProxy;
    jmethodID openCameraMethodId;
private:
    DrawerOES();
    JNIEnv *get_env();
    static DrawerOES* mDrawer;
    jmethodID updateTexImageMethodId;
    GLint  mProgram;
    GLint g_position_handle;
    GLint g_texture_coord_handler;
    GLint g_texture_handler;
    GLint g_matrix_handler;
};

#endif //CAMERA_DRAWEROES_H
