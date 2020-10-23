package com.zxx.camera.surfaceview;

import android.app.Activity;
import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import androidx.annotation.NonNull;

import com.zxx.camera.Camera.camera2Proxy;

public class Camera2SurfaceView extends SurfaceView {

    private static final String TAG = "Camera2SurfaceView";

    private camera2Proxy mCameraProxy;
    private int mRationWidth = 0;
    private int mRationHeight = 0;

    public Camera2SurfaceView(Context context) {
        this(context, null);
    }

    public Camera2SurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public Camera2SurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }
    public Camera2SurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(context);
    }

    private void init(Context context){
        getHolder().addCallback(mSurfaceHolderCallback);
        mCameraProxy = new camera2Proxy((Activity) context);
    }

    private final SurfaceHolder.Callback mSurfaceHolderCallback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(@NonNull SurfaceHolder holder) {
        mCameraProxy.setPreviewSurface(holder);
        mCameraProxy.openCamera(getWidth(),getHeight());
        }

        @Override
        public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
            int previewWidth = mCameraProxy.getPreviewSize().getWidth();
            int previewHeight = mCameraProxy.getPreviewSize().getHeight();
            if(width>height){
                setAspectRatio(previewWidth, previewHeight);
            } else {
                setAspectRatio(previewHeight, previewWidth);
            }
        }

        @Override
        public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
            mCameraProxy.releaseCamera();
        }
    };

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);
        if (0 == mRationWidth || 0 == mRationHeight) {
            setMeasuredDimension(width, height);
        } else {
            if (width < height * mRationWidth / mRationHeight) {
                setMeasuredDimension(width, width * mRationHeight / mRationWidth);
                Log.d(TAG,String.valueOf(mRationHeight)+" "+mRationWidth);
            } else {
                setMeasuredDimension(height * mRationWidth / mRationHeight, height);
                Log.d(TAG,String.valueOf(mRationHeight)+" "+mRationWidth);
            }
        }
    }

    private void setAspectRatio(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Size cannot be negative.");
        }
        mRationWidth = width;
        mRationHeight = height;
        requestLayout();
    }
    public camera2Proxy getCameraProxy() {
        return mCameraProxy;
    }
}
