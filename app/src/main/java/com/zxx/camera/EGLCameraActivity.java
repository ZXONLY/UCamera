package com.zxx.camera;

import android.graphics.SurfaceTexture;
import android.opengl.GLES20;
import android.opengl.GLES30;
import android.os.Bundle;
import android.util.Log;
import android.util.Size;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.zxx.camera.Camera.camera2Proxy;
import com.zxx.camera.Utils.EGLbase;
import com.zxx.camera.Utils.OpenGLutil;
import com.zxx.camera.renderer.CameraRender;

public class EGLCameraActivity extends AppCompatActivity {
    private SurfaceView mSurfaceview;
    private Thread mthread;
    private EGLbase mEGLbase;
    private CameraRender mcameraDrawer;
    private boolean cancle = true;
    private camera2Proxy mCameraProxy;
    private int mTextureId = -1;
    private SurfaceTexture mSurfaceTexture;
    private int mRationWidth = 0;
    private int mRationHeight = 0;
    private ImageView mCloseIv;
    private ImageView mSwitchCameraIv;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_eglbase);
        mSurfaceview  = findViewById(R.id.egl_surface_camera);
        mCameraProxy = new camera2Proxy(this);
        mCloseIv = findViewById(R.id.toolbar_close_iv_egl);
        mCloseIv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
        mSwitchCameraIv = findViewById(R.id.toolbar_switch_iv_egl);
        mSwitchCameraIv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mCameraProxy.switchCamera(mSurfaceview.getWidth(), mSurfaceview.getHeight());
                mCameraProxy.startPreview();
            }
        });
        mSurfaceview.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                mTextureId = OpenGLutil.getExternalOESTextureID();
                mSurfaceTexture = new SurfaceTexture(mTextureId);
                Log.d("TAG", "mTextureId: " + mTextureId);
                mCameraProxy.setPreviewSurface(mSurfaceTexture);
                mCameraProxy.openCamera(mSurfaceview.getWidth(),mSurfaceview.getHeight());
            }

            @Override
            public void surfaceChanged(@NonNull final SurfaceHolder holder, int format, final int width, final int height) {
                int previewWidth = mCameraProxy.getPreviewSize().getWidth();
                int previewHeight = mCameraProxy.getPreviewSize().getHeight();
                mthread = new Thread(){
                    @Override
                    public void run() {
                        super.run();
                        mEGLbase = new EGLbase();
                        Size pre = mCameraProxy.getPreviewSize();
                        mEGLbase.render(holder.getSurface(), pre.getWidth(), pre.getHeight());
                        while (!cancle) {
                            GLES20.glViewport(0, 0, width, height);
                            GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);
                            GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                            mSurfaceTexture.updateTexImage();
                            mcameraDrawer = new CameraRender();
                            mcameraDrawer.setTexture(mTextureId);
                            if(width>height){
                                mcameraDrawer.draw(mCameraProxy.isFront(),pre.getWidth(),pre.getHeight(),width,height,mCameraProxy.getOrientation());
                                Log.i("hhhh",mCameraProxy.getOrientation()+"");
                            } else {
                                mcameraDrawer.draw(mCameraProxy.isFront(),pre.getHeight(),pre.getWidth(),width,height,mCameraProxy.getOrientation());
                                Log.i("wwwww",mCameraProxy.getOrientation()+"");
                            }
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
                mCameraProxy.releaseCamera();
                mEGLbase.releaseEGL();
            }
        });


    }


    private void setAspectRatio(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Size cannot be negative.");
        }
        mRationWidth = width;
        mRationHeight = height;
        mSurfaceview.requestLayout();
    }
}
