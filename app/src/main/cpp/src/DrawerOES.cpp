#include <DrawerOES.h>
#include <GLES2/gl2ext.h>
//
// Created by bytedance on 29/10/2020.
//
//const char *vertexShaderSource = "#version 300 es\nuniform mat4 uMVPMatrix;\nlayout (location = 0) in vec4 vPosition;\nlayout (location = 1) in vec2 aTextureCoord;\nout vec2 yuvTexCoords;\n\"void main() { \ngl_Position  = uMVPMatrix * vPosition;\ngl_PointSize = 10.0;\nyuvTexCoords = aTextureCoord;}";
//const char *fragmentShaderSource ="#version 300 es\n#extension GL_OES_EGL_image_external_essl3 : require\nprecision mediump float;\nuniform samplerExternalOES yuvTexSampler;\nin vec2 yuvTexCoords;\nout vec4 vFragColor;\nvoid main() {\nvFragColor = texture(yuvTexSampler,yuvTexCoords);\n}";

const GLfloat squareCoords[8] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f,  1.0f,
};

const GLfloat textureVertices[8] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f,1.0f
};
const GLbyte VERTEX_ORDER[4] = {0,1,2,3};
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
    g_position_handle = glGetAttribLocation(mProgram,"vPosition");
    g_texture_handler = glGetAttribLocation(mProgram,"yuvTexSampler");
    g_texture_coord_handler = glGetAttribLocation(mProgram,"aTextureCoord");
    g_matrix_handler = glGetAttribLocation(mProgram,"uMVPMatrix");

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
void DrawerOES::draw(GLuint texture, float *mMatrix) {
    glClear(GL_COLOR_BUFFER_BIT);
// 1. 选择使用的程序
    glUseProgram(mProgram);
//2.绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,texture);
//3.加载顶点数据
    glVertexAttribPointer(g_position_handle,2,GL_FLOAT,false,2*4,squareCoords);
    glEnableVertexAttribArray(g_position_handle);

    glVertexAttribPointer(g_texture_coord_handler,2,GL_FLOAT, false,2*4,textureVertices);
    glEnableVertexAttribArray(g_texture_coord_handler);

    glUniformMatrix4fv(g_matrix_handler,1, false,mMatrix);
    glUniform1i(g_texture_handler,0);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    glDisableVertexAttribArray(g_position_handle);
    glDisableVertexAttribArray(g_texture_coord_handler);
}
void DrawerOES::draw(GLuint texture) {
    glClear(GL_COLOR_BUFFER_BIT);
// 1. 选择使用的程序
    glUseProgram(mProgram);
    //update();
//2.绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,texture);
//3.加载顶点数据
    glVertexAttribPointer(g_position_handle,2,GL_FLOAT,false,2*4,squareCoords);
    glEnableVertexAttribArray(g_position_handle);

    glVertexAttribPointer(g_texture_coord_handler,2,GL_FLOAT, false,2*4,textureVertices);
    glEnableVertexAttribArray(g_texture_coord_handler);

    float mMatrix[] = {
            1.4541668,0,0,0,
            0,1,0,0,
            0,0,-1,0,
            0,0,-1.0,1
    };

    glUniformMatrix4fv(g_matrix_handler,1, false,mMatrix);
    glUniform1i(g_texture_handler,0);
   glDrawElements(GL_TRIANGLE_FAN,4,GL_BYTE,VERTEX_ORDER);
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
