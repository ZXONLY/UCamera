package com.zxx.camera.View;

import android.content.Context;
import android.opengl.EGLContext;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

import com.zxx.camera.Utils.EGLbase;

import java.lang.ref.WeakReference;

public class EglSurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    private final String TAG = "EglSurfaceView";
    private Surface mSurface;
    private EGLContext mEglContext;
    private Render mRender;
    private EGLThread mEGLThread;

    public EglSurfaceView(Context context) {
        this(context, null);
    }

    public EglSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public EglSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }
    private void init() {
        SurfaceHolder holder = getHolder();
        holder.addCallback(this);
    }

    private void setSurface(Surface surface){
        mSurface = surface;
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.d(TAG,"surfaceCreated");
        if (mSurface == null) {
            mSurface = holder.getSurface();
        }
        mEGLThread = new EGLThread(new WeakReference<>(this));
        mEGLThread.isCreate = true;
        mEGLThread.start();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        mEGLThread.width = width;
        mEGLThread.height = height;
        mEGLThread.isChange = true;
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        mEGLThread.onDestroy();
        mEGLThread = null;
        mSurface = null;
        mEglContext = null;
    }

    public void setRender(Render render){
        mRender = render;
    }

    public EGLContext getEglContext() {
        if (mEGLThread != null) {
            return mEGLThread.getEglContex();
        }
        return null;
    }


    private class EGLThread extends Thread{

        private EGLbase mEGLbase;
        private WeakReference<EglSurfaceView> mEGLSurfaceViewWeakRef;

        int width;
        int height;

        private boolean isExit = false;
        private boolean isCreate;
        private boolean isChange;
        private boolean isStart = false;

        public EGLThread(WeakReference<EglSurfaceView> eGLSurfaceViewWeakRef){
            this.mEGLSurfaceViewWeakRef = eGLSurfaceViewWeakRef;
        }

        @Override
        public void run(){
            super.run();
            try{
                guardedRun();
            }catch (Exception e){

            }
        }

        private void guardedRun(){
            isExit = false;
            isStart = false;
            mEGLbase = new EGLbase();
            mEGLbase.render(mEGLSurfaceViewWeakRef.get().mSurface,width,height);
            Log.d(TAG,"guardedRun 1");
            while (true){
                if(isExit){
                    //TODO 释放资源
                    break;
                }
                Log.d(TAG,"guardedRun 2");

                onCreate();
                onChange(width,height);
                onDraw();
                isStart = true;
            }
        }
        private void onCreate(){
            if(!isCreate || mEGLSurfaceViewWeakRef.get().mRender==null)
                return;
            isCreate = false;
            Log.d(TAG,"onCreate");
            mEGLSurfaceViewWeakRef.get().mRender.onSurfaceCreate();
        }

        private void onChange(int width,int height){
            //TODO
            if(!isChange || mEGLSurfaceViewWeakRef.get().mRender==null)
                return;
            isChange = false;
            Log.d(TAG,"onChange");
            mEGLSurfaceViewWeakRef.get().mRender.onSurfaceChanged(width,height);
        }
        private void onDraw(){
            //TODO
            Log.d(TAG,"onDraw1");
            if(mEGLSurfaceViewWeakRef.get().mRender==null)
                return;
            Log.d(TAG,"onDraw2");
            mEGLSurfaceViewWeakRef.get().mRender.onDrawFrame();
            if(!isStart){
                mEGLSurfaceViewWeakRef.get().mRender.onDrawFrame();
            }
            Log.d(TAG,"onDraw3");
            mEGLbase.swapBuffers();
        }

        void requrstRender(){
            //TODO
        }
        void release(){//TODO
        }
        EGLContext getEglContex(){
            if(mEGLbase!=null){
                return mEGLbase.getEGLContext();
            }
            return null;
        }
        void onDestroy(){
            //TODO
            mEGLbase.releaseEGL();
        }
    }

    public interface Render{
        void onSurfaceCreate();

        void onSurfaceChanged(int width,int height);

        void onDrawFrame();
    }
}
