package com.zxx.camera.renderer;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.SurfaceTexture;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;

import com.zxx.camera.Camera.camera2Proxy;
import com.zxx.camera.Utils.OpenGLutil;

public class NativeCameraRender implements SurfaceHolder.Callback{

    private final String TAG = "NativeCameraRender";

    private int mTextureId = -1;
    //private SurfaceTexture mSurfaceTexture;
    private camera2Proxy mCameraProxy;
    private Context mContext;
    private int mwidth;
    private int mheight;
    private Size pre;

    public void init(Context context,int width,int height){
        mContext = context;
        mCameraProxy = new camera2Proxy((Activity) mContext);
        mheight = height;
        mwidth = width;
        //mTextureId = OpenGLutil.getExternalOESTextureID();
        //mSurfaceTexture = new SurfaceTexture(mTextureId);
        mCameraProxy.setPreviewSurface(nativeGetSurfaceTexture());
    }

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.i(TAG,"surfaceCreated");
        registerAssetManager(mContext.getAssets());
        nativeInit(holder.getSurface(),mCameraProxy,mwidth,mheight);
        //mCameraProxy.openCamera(mwidth,mheight);
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
//        mSurfaceTexture.updateTexImage();
        surfaceChanged(width,height);
        Log.i(TAG,"onDrawFrame2");
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    private static native void registerAssetManager(AssetManager assetManager);
    private static native void nativeInit(Surface surfac,camera2Proxy mCameraProxy,int mwidth,int mheight);
    private static native void surfaceChanged(int width,int height);
    private static native SurfaceTexture nativeGetSurfaceTexture();
}
