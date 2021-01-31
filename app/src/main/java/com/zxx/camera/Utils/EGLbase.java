package com.zxx.camera.Utils;

import android.annotation.TargetApi;
import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLExt;
import android.opengl.EGLSurface;
import android.os.Build;
import android.util.Log;
import android.view.Display;
import android.view.Surface;

public class EGLbase {//使用的是EGL14
    private final String TAG = "EGLbase";
    private EGLDisplay mEGLDisplay;
    private int[] version = new int[2];
    private EGLConfig mEGLConfig;
    private EGLContext mEGLContext;
    private EGLSurface mEglSurface;
    private int mGlVersion = -1;

    public EGLbase(){
        mEGLDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        if(mEGLDisplay == EGL14.EGL_NO_DISPLAY){
            throw new RuntimeException("egl no display");
        }
        if(!EGL14.eglInitialize(mEGLDisplay,version,0,version,1)){
            mEGLDisplay = null;
            throw new RuntimeException("egl no display");
        }
        //Try to get a GLES3 context


        mEGLConfig = getConfig(true);
        if(mEGLConfig!=null){
            int[] contexAttribList = {
                    EGL14.EGL_CONTEXT_CLIENT_VERSION,3,
                    EGL14.EGL_NONE
            };
            mEGLContext = EGL14.eglCreateContext(mEGLDisplay,mEGLConfig,EGL14.EGL_NO_CONTEXT,contexAttribList,0);
            if(mEGLContext==EGL14.EGL_NO_CONTEXT){
                Log.w(TAG,"can't get egl version 3" + EGL14.eglGetError());
            }else {
                mGlVersion  = 3;
                Log.i(TAG,"get egl version 3" + EGL14.eglGetError());
            }
        }
        if(mEGLContext==EGL14.EGL_NO_CONTEXT){
            mEGLConfig = getConfig(false);
            int[] contexAttribList = {
                    EGL14.EGL_CONTEXT_CLIENT_VERSION,2,
                    EGL14.EGL_NONE
            };
            mEGLContext = EGL14.eglCreateContext(mEGLDisplay,mEGLConfig,EGL14.EGL_NO_CONTEXT,contexAttribList,0);
            if(mEGLContext==EGL14.EGL_NO_CONTEXT){
                throw new RuntimeException("egl error:"+EGL14.eglGetError());
            }
            mGlVersion = 2;
        }
    }

    public EGLSurface render(Surface surface){
        int[] surfaceAttribList = {
                EGL14.EGL_NONE
        };
        mEglSurface = EGL14.eglCreateWindowSurface(mEGLDisplay,mEGLConfig,surface,surfaceAttribList,0);
        makeCurrent(mEglSurface);
        return mEglSurface;
    }

    public EGLSurface render(Surface surface, int width, int height){
        return render(surface);
    }

    private EGLConfig getConfig(boolean isGles3){
        int renderType = EGL14.EGL_OPENGL_ES2_BIT;
        if(isGles3){
            renderType |=EGLExt.EGL_OPENGL_ES3_BIT_KHR;
        }
        int[] attribList = {
                EGL14.EGL_RED_SIZE,8,//颜色缓冲区中红色的位数
                EGL14.EGL_GREEN_SIZE,8,
                EGL14.EGL_BLUE_SIZE,8,
                EGL14.EGL_ALPHA_SIZE,8,
                EGL14.EGL_RENDERABLE_TYPE,renderType,//渲染窗口支持的布局组成
                EGL14.EGL_SURFACE_TYPE,EGL14.EGL_PBUFFER_BIT, //egl支持的窗口类型
                EGL14.EGL_NONE
        };
//        if ((flags & FLAG_RECORDABLE) != 0) {
//            attribList[attribList.length - 3] = EGL_RECORDABLE_ANDROID;
//            attribList[attribList.length - 2] = 1;
//        }
        EGLConfig[] configs = new EGLConfig[1];
        int[] num_config = new int[1];
        if(!EGL14.eglChooseConfig(mEGLDisplay,attribList,0,configs,0,configs.length,num_config,0)){
            Log.w(TAG, "unable to find RGB8888 / " + version + " EGLConfig");
            return null;
        }
        return configs[0];
    }

    public void releaseSurface(EGLSurface eglSurface){
        EGL14.eglDestroySurface(mEGLDisplay,eglSurface);
    }
    public EGLSurface createOffscreenSurface(int width,int height){
        int[] surfaceAttribs = {
                EGL14.EGL_WIDTH,width,
                EGL14.EGL_HEIGHT,height,
                EGL14.EGL_NONE
        };
        EGLSurface eglSurface = EGL14.eglCreatePbufferSurface(mEGLDisplay,mEGLConfig,surfaceAttribs,0);
        checkEglError("eglCreatePbufferSurface");
        if(eglSurface==null){
            throw new RuntimeException("surface was null");
        }
        return eglSurface;
    }

    public void makeCurrent(){
        makeCurrent(mEglSurface);
    }

    public void makeCurrent(EGLSurface eglSurface){
        if(mEGLDisplay==EGL14.EGL_NO_DISPLAY){
            Log.d(TAG, "NOTE: makeCurrent w/o display");
        }
        if(!EGL14.eglMakeCurrent(mEGLDisplay,eglSurface,eglSurface,mEGLContext)){
            throw new RuntimeException("eglMakeCurrent(draw,read) failed");
        }
    }
    public void makeCurrent(EGLSurface drawSurface, EGLSurface readSurface){
        if(mEGLDisplay==EGL14.EGL_NO_DISPLAY){
            Log.d(TAG, "NOTE: makeCurrent w/o display");
        }
        if(!EGL14.eglMakeCurrent(mEGLDisplay,drawSurface,readSurface,mEGLContext)){
            throw new RuntimeException("eglMakeCurrent(draw,read) failed");
        }
    }
    public void makeNothingCurrent() {
        if (!EGL14.eglMakeCurrent(mEGLDisplay, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_SURFACE,
                EGL14.EGL_NO_CONTEXT)) {
            throw new RuntimeException("eglMakeCurrent failed");
        }
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
            makeNothingCurrent();
            releaseSurface(mEglSurface);
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
    /**
     * Sends the presentation time stamp to EGL.  Time is expressed in nanoseconds.
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public void setPresentationTime(long nsecs) {
        EGLExt.eglPresentationTimeANDROID(mEGLDisplay, mEglSurface, nsecs);
        checkEglError("eglPresentationTimeANDROID");
    }
    public int getGlVersion() {
        return mGlVersion;
    }
    private void checkEglError(String msg) {
        int error;
        if ((error = EGL14.eglGetError()) != EGL14.EGL_SUCCESS) {
            throw new RuntimeException(msg + ": EGL error: 0x" + Integer.toHexString(error));
        }
    }

    public EGLContext getEGLContext(){
        return mEGLContext;
    }
}
