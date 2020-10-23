package com.zxx.camera.Camera;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Display;
import android.view.OrientationEventListener;
import android.view.Surface;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;

import java.util.Arrays;

public class camera2Proxy {

    private static final String TAG = "Camera2Proxy";
    private Size mPreviewSize;// 预览大小
    private CameraDevice mCameraDevice;//相机对象
    private int mCameraId = CameraCharacteristics.LENS_FACING_FRONT;// 要打开的摄像头ID
    private CameraCharacteristics mCameraCharacteristics;// 相机属性
    private CameraManager mCameraManager = null;
    private CameraCaptureSession mCameraCaptureSession;
    private Activity mActivity;//获取CameraManager需要activity
    private OrientationEventListener mOrientationEventListener;
    private HandlerThread mBackgroundThread;
    private Handler mBackgroundHandler;
    private ImageReader mImageReader;
    private Surface mPerviewSurface;//根据mPreviewSurfaceTexture创建
    private SurfaceTexture mPreviewSurfaceTexture;
    private CaptureRequest mCaptureRequest;
    private CaptureRequest.Builder mCaptureRequestBuilder;// 相机预览请求的构造器
    private Display mDisplay;
    private ScreenOrientationDetector mScreenOrientationDetector;


    private int mDeviceOrientation = 0;


    private CameraDevice.StateCallback mstateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice camera) {
            Log.d(TAG, mCameraId + ": onOpened");
            mCameraDevice = camera;
            initPreviewRequest();
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice camera) {
            Log.d(TAG, mCameraId + ": onDisconnected");
            releaseCamera();
        }

        @Override
        public void onError(@NonNull CameraDevice camera, int error) {
            Log.d(TAG, mCameraId + ": onError");
            releaseCamera();
        }
    };

    public camera2Proxy(Activity activity) {
        mActivity = activity;
        mCameraManager = (CameraManager) mActivity.getSystemService(Context.CAMERA_SERVICE);
        mOrientationEventListener = new OrientationEventListener(mActivity) {
            @Override
            public void onOrientationChanged(int orientation) {
                mDeviceOrientation = orientation;
            }
        };
    }

    @SuppressLint("MissingPermission")
    public void openCamera(int width,int height) {
        Log.v(TAG, "openCamera");
        startBackgroundThread();
        mOrientationEventListener.enable();
        try {
            mCameraCharacteristics = mCameraManager.getCameraCharacteristics(Integer.toString(mCameraId));
            StreamConfigurationMap streamConfigurationMap = mCameraCharacteristics.get(CameraCharacteristics
                    .SCALER_STREAM_CONFIGURATION_MAP);
            //预览大小
            //Log.d(TAG, "preview size: " + mPreviewSize.getWidth() + "*" + mPreviewSize.getHeight());
            mPreviewSize = chooseOptimalSize(streamConfigurationMap.getOutputSizes(SurfaceTexture.class),width,height);
            //mPreviewSize = streamConfigurationMap.getOutputSizes(SurfaceTexture.class)[2];
            Log.d(TAG, "preview size: " + mPreviewSize.getWidth() + "*" + mPreviewSize.getHeight());
            mImageReader = ImageReader.newInstance(mPreviewSize.getWidth(),mPreviewSize.getHeight(),ImageFormat.JPEG,2);
            Log.d(TAG, "preview size: " + mPreviewSize.getWidth() + "*" + mPreviewSize.getHeight());
            //打开camera
            mCameraManager.openCamera(Integer.toString(mCameraId), mstateCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    public void releaseCamera(){
        //关闭CameraCaptureSession
        if(mCameraCaptureSession!=null)
            mCameraCaptureSession.close();
        //关闭CameraDevice
        if(mCameraDevice!=null)
            mCameraDevice.close();
        //关闭图片预览
        if(mImageReader!=null)
            mImageReader.close();
        mOrientationEventListener.disable();
        //关闭后台线程
        stopBackgroundThread();
    }

    public void setPreviewSurface(SurfaceHolder holder){
        mPerviewSurface = holder.getSurface();
    }

    public void setPreviewSurface(SurfaceTexture surfaceTexture) {
        mPreviewSurfaceTexture = surfaceTexture;
    }

    public void setImageAvailableListener(ImageReader.OnImageAvailableListener listener){
        if(mImageReader==null){
            Log.w(TAG,"setImageAvailableListener: mImageReader is null");
            return;
        }
        mImageReader.setOnImageAvailableListener(listener,null);
    }


    public void stopPreview(){
        Log.v(TAG, "stopPreview");
        if (mCameraCaptureSession == null || mCaptureRequestBuilder == null) {
            Log.w(TAG, "stopPreview: mCaptureSession or mPreviewRequestBuilder is null");
            return;
        }
        try {
            mCameraCaptureSession.stopRepeating();
        }catch (CameraAccessException e){
            e.printStackTrace();
        }

    }

    public void startPreview() {
        Log.v(TAG, "startPreview");
        if (mCameraCaptureSession == null || mCaptureRequestBuilder == null) {
            Log.w(TAG, "startPreview: mCaptureSession or mPreviewRequestBuilder is null");
            return;
        }
        try {
            mCameraCaptureSession.setRepeatingRequest(mCaptureRequest, null, mBackgroundHandler);
        }catch (CameraAccessException e){
            e.printStackTrace();
        }
    }

    public void switchCamera(){
        //TODO
    }


    public void initPreviewRequest() {
        try {
            mCaptureRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            if(mPerviewSurface!=null&&mPreviewSurfaceTexture!=null){
                mPreviewSurfaceTexture.setDefaultBufferSize(mPreviewSize.getWidth(),mPreviewSize.getHeight());
                mPerviewSurface = new Surface(mPreviewSurfaceTexture);
            }
            mCaptureRequestBuilder.addTarget(mPerviewSurface);
            mCameraDevice.createCaptureSession(Arrays.asList(mPerviewSurface, mImageReader.getSurface()),
                    new CameraCaptureSession.StateCallback() {

                        @Override
                        public void onConfigured(@NonNull CameraCaptureSession session) {
                            mCameraCaptureSession = session;
                            // 设置连续自动对焦
                            mCaptureRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest
                                    .CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                            // 设置自动曝光
                            mCaptureRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest
                                    .CONTROL_AE_MODE_ON_AUTO_FLASH);
                            // 设置完后自动开始预览
                            mCaptureRequest = mCaptureRequestBuilder.build();
                            startPreview();
                        }

                        @Override
                        public void onConfigureFailed(@NonNull CameraCaptureSession session) {
                            Log.e(TAG, "ConfigureFailed. session: mCaptureSession");
                        }
                    }, mBackgroundHandler); // handle 传入 null 表示使用当前线程的 Looper
        }catch (CameraAccessException e){
            e.printStackTrace();
        }
    }

    private void startBackgroundThread(){
        if(mBackgroundHandler==null||mBackgroundThread==null){
            Log.v(TAG, "startBackgroundThread");
            mBackgroundThread = new HandlerThread("CameraBackground");
            mBackgroundThread.start();
            mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
        }
    }

    private void stopBackgroundThread(){
        Log.v(TAG, "stopBackgroundThread");
        mBackgroundThread.quitSafely();
        try{
            mBackgroundThread.join();
            mBackgroundThread = null;
            mBackgroundHandler = null;
        }catch (InterruptedException e){
            e.printStackTrace();
        }
    }


    private Size chooseOptimalSize(Size[] sizes,int viewWidth, int viewHeight){
        // 1. 确定期望的宽高
        int desiredWidth;
        int desiredHeight;
        // 横屏状态, 直接使用 view 的宽高
        if (isLandscape(screenOrientationDegrees)) {
            desiredWidth = viewWidth;
            desiredHeight = viewHeight;
        }
        // 竖屏状态, 反转宽高
        else {
            desiredWidth = viewHeight;
            desiredHeight = viewWidth;
        }

        Size result = null;
        for (Size size : sizes) {// sizes 已按照从小到大排序
            result = size;
            // 选取宽高都比期望稍大的
            if (desiredWidth <= size.getWidth() && desiredHeight <= size.getHeight()) {
                return size;
            }
        }
        return result;
    }

    public Size getPreviewSize(){
        return mPreviewSize;
    }
}
