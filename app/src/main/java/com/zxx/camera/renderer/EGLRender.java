package com.zxx.camera.renderer;

import android.app.Activity;
import android.content.Context;
import android.graphics.SurfaceTexture;
import android.util.Log;
import android.util.Size;

import com.zxx.camera.Camera.camera2Proxy;
import com.zxx.camera.Utils.OpenGLutil;
import com.zxx.camera.View.EglSurfaceView;

public class EGLRender implements EglSurfaceView.Render {

    private final String TAG = "EGLRender";

    private int mTextureId = -1;
    private SurfaceTexture mSurfaceTexture;
    private camera2Proxy mCameraProxy;
    private Context mContext;
    private CameraRender mcameraRender;
    private int mwidth;
    private int mheight;
    private Size pre;

    public void init(Context context,int width,int height){
        mContext = context;
        mCameraProxy = new camera2Proxy((Activity) mContext);
        mheight = height;
        mwidth = width;
        mTextureId = OpenGLutil.getExternalOESTextureID();
        mSurfaceTexture = new SurfaceTexture(mTextureId);
        mCameraProxy.setPreviewSurface(mSurfaceTexture);
    }


    @Override
    public void onSurfaceCreate() {
        mCameraProxy.openCamera(mwidth,mheight);
        mcameraRender = new CameraRender();
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        pre = mCameraProxy.getPreviewSize();
        mwidth = width;
        mheight = height;
        mSurfaceTexture.updateTexImage();
        mcameraRender.setTexture(mTextureId);
        mcameraRender.draw(mCameraProxy.isFront(),pre.getHeight(),pre.getWidth(),width,height,mCameraProxy.getOrientation());
    }

    @Override
    public void onDrawFrame() {
        Log.i(TAG,"onDrawFrame1");
        mSurfaceTexture.updateTexImage();
        mcameraRender.setTexture(mTextureId);
        mcameraRender.draw(mCameraProxy.isFront(),pre.getHeight(),pre.getWidth(),mwidth,mheight,mCameraProxy.getOrientation());
        Log.i(TAG,"onDrawFrame2");
    }
}
