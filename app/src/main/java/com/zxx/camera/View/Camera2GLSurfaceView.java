package com.zxx.camera.View;

import android.app.Activity;
import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;

import com.zxx.camera.Camera.camera2Proxy;
import com.zxx.camera.Utils.OpenGLutil;
import com.zxx.camera.renderer.CameraRender;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class Camera2GLSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {

    private static final String TAG = "Camera2GLSurfaceView";
    private camera2Proxy mCameraProxy;
    private SurfaceTexture mSurfaceTexture;
    private CameraRender mDrawer;
    private int mRatioWidth = 0;
    private int mRatioHeight = 0;
    private float mOldDistance;
    private int mTextureId = -1;

    public Camera2GLSurfaceView(Context context) {
        this(context, null);
    }

    public Camera2GLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    private void init(Context context) {
        mCameraProxy = new camera2Proxy((Activity) context);
        setEGLContextClientVersion(3);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mTextureId = OpenGLutil.getExternalOESTextureID();
        mSurfaceTexture = new SurfaceTexture(mTextureId);
        mSurfaceTexture.setOnFrameAvailableListener(this);
        mCameraProxy.setPreviewSurface(mSurfaceTexture);
        mDrawer = new CameraRender();
        Log.d(TAG, "onSurfaceCreated. width: " + getWidth() + ", height: " + getHeight());
        mCameraProxy.openCamera(getWidth(), getHeight());
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.d(TAG, "onSurfaceChanged. thread: " + Thread.currentThread().getName());
        Log.d(TAG, "onSurfaceChanged. width: " + width + ", height: " + height);
        int previewWidth = mCameraProxy.getPreviewSize().getWidth();
        int previewHeight = mCameraProxy.getPreviewSize().getHeight();
        if (width > height) {
            setAspectRatio(previewWidth, previewHeight);
        } else {
            setAspectRatio(previewHeight, previewWidth);
        }
        GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        GLES20.glClearColor(0, 0, 0, 0);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
        mSurfaceTexture.updateTexImage();
        mDrawer.setTexture(mTextureId);
        //mDrawer.draw();
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        requestRender();
    }

    private void setAspectRatio(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Size cannot be negative.");
        }
        mRatioWidth = width;
        mRatioHeight = height;
        post(new Runnable() {
            @Override
            public void run() {
                requestLayout(); // must run in UI thread
            }
        });
    }

    public camera2Proxy getCameraProxy() {
        return mCameraProxy;
    }

    public SurfaceTexture getSurfaceTexture() {
        return mSurfaceTexture;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);
        if (0 == mRatioWidth || 0 == mRatioHeight) {
            setMeasuredDimension(width, height);
        } else {
            if (width < height * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth);
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height);
            }
        }
    }


    private static float getFingerSpacing(MotionEvent event) {
        float x = event.getX(0) - event.getX(1);
        float y = event.getY(0) - event.getY(1);
        return (float) Math.sqrt(x * x + y * y);
    }

}

