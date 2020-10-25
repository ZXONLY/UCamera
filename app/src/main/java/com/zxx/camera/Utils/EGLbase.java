package com.zxx.camera.Utils;

import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;
import android.view.Surface;


public class EGLbase {//使用的是EGL14
    private EGLDisplay mEGLDisplay;
    private int[] version = new int[2];
    private EGLConfig mEGLConfig;
    private EGLContext mEGLContext;
    private EGLSurface mEglSurface;
    private int[] attrList = {
            EGL14.EGL_RED_SIZE,8,//颜色缓冲区中红色的位数
            EGL14.EGL_GREEN_SIZE,8,
            EGL14.EGL_BLUE_SIZE,8,
            EGL14.EGL_ALPHA_SIZE,8,
            EGL14.EGL_RENDERABLE_TYPE,EGL14.EGL_OPENGL_ES2_BIT,//渲染窗口支持的布局组成
            EGL14.EGL_SURFACE_TYPE,EGL14.EGL_PBUFFER_BIT, //egl支持的窗口类型
            EGL14.EGL_NONE
    };


    public EGLbase(){
        mEGLDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        if(mEGLDisplay == EGL14.EGL_NO_DISPLAY){
            throw new RuntimeException("egl no display");
        }
        if(!EGL14.eglInitialize(mEGLDisplay,version,0,version,1)){
            mEGLDisplay = null;
            throw new RuntimeException("egl no display");
        }
        EGLConfig[] configs = new EGLConfig[1];
        int[] num_config = new int[1];
        if(!EGL14.eglChooseConfig(mEGLDisplay,attrList,0,configs,0,configs.length,num_config,0)){
            throw new RuntimeException("unable to find RGB888 ES2 EGL config");
        }
//        int numConfigs = num_config[0];
//        if(numConfigs<=0){
//            throw new IllegalArgumentException("No configs match configSpec");
//        }
        mEGLConfig = configs[0];
        int[] contexAttribList = {
                EGL14.EGL_CONTEXT_CLIENT_VERSION,2,
                EGL14.EGL_NONE
        };
        mEGLContext = EGL14.eglCreateContext(mEGLDisplay,mEGLConfig,EGL14.EGL_NO_CONTEXT,contexAttribList,0);
        if(mEGLContext==EGL14.EGL_NO_CONTEXT){
            throw new RuntimeException("egl error:"+EGL14.eglGetError());
        }
    }

    public EGLSurface render(Surface surface, int width, int height){
        int[] surfaceAttribList = {EGL14.EGL_NONE};
        mEglSurface = EGL14.eglCreateWindowSurface(mEGLDisplay,mEGLConfig,surface,surfaceAttribList,0);
        EGL14.eglMakeCurrent(mEGLDisplay,mEglSurface,mEglSurface,mEGLContext);
        return mEglSurface;
    }


    public boolean swapBuffers() {
        if (mEGLDisplay != null){
            return EGL14.eglSwapBuffers(mEGLDisplay,mEglSurface);
        }else {
            throw new RuntimeException("egl is null");
        }
    }
    public void releaseEGL(){
        if(mEglSurface!=null&&mEglSurface!=EGL14.EGL_NO_SURFACE){
            EGL14.eglMakeCurrent(mEGLDisplay,EGL14.EGL_NO_SURFACE,EGL14.EGL_NO_SURFACE,EGL14.EGL_NO_CONTEXT);
            EGL14.eglDestroySurface(mEGLDisplay,mEglSurface);
            mEglSurface = null;
        }

        if(mEGLContext!=null){
            EGL14.eglDestroyContext(mEGLDisplay,mEGLContext);
            mEGLContext = null;
        }
        if (mEGLDisplay != null) {
            EGL14.eglTerminate(mEGLDisplay);
            mEGLDisplay = null;
        }
    }
}
