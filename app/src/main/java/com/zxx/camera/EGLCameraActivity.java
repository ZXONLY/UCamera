package com.zxx.camera;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.opengl.GLES30;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.zxx.camera.Camera.camera2Proxy;
import com.zxx.camera.Utils.EGLbase;
import com.zxx.camera.Utils.OpenGLutil;
import com.zxx.camera.glSurfaceview.CameraDrawer;
import com.zxx.camera.renderer.TriangleRenderer;

public class EGLCameraActivity extends AppCompatActivity {
    private SurfaceView mSurfaceview;
    private Thread mthread;
    private Handler mBackgroundHandler;
    private HandlerThread mBackgroundThread;
    private EGLbase mEGLbase;
    private CameraDrawer mcameraDrawer;
    private boolean cancle = true;
    private camera2Proxy mCameraProxy;
    private Activity mactivity;
    private int mTextureId = -1;
    private SurfaceTexture mSurfaceTexture;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_eglbase);
        mactivity = this;
        mSurfaceview  = findViewById(R.id.egl_surface_camera);
        mCameraProxy = new camera2Proxy(mactivity);

        mSurfaceview.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                mTextureId = OpenGLutil.getExternalOESTextureID();
                mSurfaceTexture = new SurfaceTexture(mTextureId);
                mCameraProxy.setPreviewSurface(mSurfaceTexture);
                mcameraDrawer = new CameraDrawer();
                //Log.d(TAG, "onSurfaceCreated. width: " + getWidth() + ", height: " + getHeight());
                mCameraProxy.openCamera(mSurfaceview.getWidth(),mSurfaceview.getHeight());
//                mBackgroundThread = new HandlerThread("CameraBackground");
//                mBackgroundThread.start();
//                mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
            }

            @Override
            public void surfaceChanged(@NonNull final SurfaceHolder holder, int format, final int width, final int height) {
                mthread = new Thread(){
                    @Override
                    public void run() {
                        super.run();
                        mEGLbase = new EGLbase();
                        mEGLbase.render(holder.getSurface(), width, height);
                        while (!cancle) {
                            GLES30.glViewport(0, 0, width, height);
                            GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);
                            GLES30.glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
                            mSurfaceTexture.updateTexImage();
                            mcameraDrawer.setTexture(mTextureId);
                            mcameraDrawer.draw();
                            mEGLbase.swapBuffers();

                            try {
                                Thread.sleep(16);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                };
                cancle  = false;
                mthread.start();
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                try {
                    cancle = true;
                    mthread.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                mEGLbase.releaseEGL();
            }
        });
    }
//
//    private class preview implements Runnable{
//
//        private SurfaceHolder holder;
//
//        @Override
//        public void run() {
//            mEGLbase = new EGLbase();
//            mEGLbase.render(holder.getSurface(),width,height);
//            GLES30.glViewport(0, 0, width, height);
//            GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);
//            GLES30.glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
//            mSurfaceTexture.updateTexImage();
//            mcameraDrawer.setTexture(mTextureId);
//            mcameraDrawer.draw();
//            mEGLbase.swapBuffers();
//        }
//    }
}
