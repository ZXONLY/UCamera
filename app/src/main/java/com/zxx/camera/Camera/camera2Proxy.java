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
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Display;
import android.view.OrientationEventListener;
import android.view.Surface;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

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
    private CaptureRequest mPreviewRequest;
    private CaptureRequest.Builder mPreviewRequestBuilder;// 相机预览请求的构造器
    private Display mDisplay;
    private ScreenOrientationDetector mScreenOrientationDetector;

    private static final String GALLERY_PATH = Environment.getExternalStoragePublicDirectory(Environment
            .DIRECTORY_DCIM) + File.separator + "Camera";
    private static final SimpleDateFormat DATE_FORMAT = new SimpleDateFormat("yyyyMMdd_HHmmss");

    private int mState = CameraState.STATE_PREVIEW;

    private Semaphore mCameraOpenCloseLock = new Semaphore(1);


    private int mDeviceOrientation = 0;


    private CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice camera) {
            Log.d(TAG, mCameraId + ": onOpened");
            mCameraOpenCloseLock.release();
            mCameraDevice = camera;
            initPreviewRequest();
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice camera) {
            Log.d(TAG, mCameraId + ": onDisconnected");
            mCameraOpenCloseLock.release();
            releaseCamera();
        }

        @Override
        public void onError(@NonNull CameraDevice camera, int error) {
            Log.d(TAG, mCameraId + ": onError"+error);
            mCameraOpenCloseLock.release();
            releaseCamera();
        }

    };

    public camera2Proxy(Activity activity) {
        mActivity = activity;
        mCameraManager = (CameraManager) mActivity.getSystemService(Context.CAMERA_SERVICE);
        mDisplay = mActivity.getDisplay();
        mScreenOrientationDetector = new ScreenOrientationDetector(mActivity,new ScreenOrientationDetector.OnDisplayChangedListener(){
            @Override
            public void onDisplayOrientationChanged(int displayOrientation) {
                Log.d(TAG,"DisplayOrientation: "+String.valueOf(displayOrientation));

            }
        });

    }

    @SuppressLint("MissingPermission")
    public void openCamera(int width,int height) {
        Log.v(TAG, "openCamera");
        startBackgroundThread();
        mScreenOrientationDetector.enable(mDisplay);
        try {
            mCameraCharacteristics = mCameraManager.getCameraCharacteristics(Integer.toString(mCameraId));
            StreamConfigurationMap streamConfigurationMap = mCameraCharacteristics.get(CameraCharacteristics
                    .SCALER_STREAM_CONFIGURATION_MAP);
            //预览大小
            //Log.d(TAG, "preview size: " + mPreviewSize.getWidth() + "*" + mPreviewSize.getHeight());
            mPreviewSize = chooseOptimalSize(streamConfigurationMap.getOutputSizes(SurfaceTexture.class),width,height);
            Log.d(TAG, "preview size: " + mPreviewSize.getWidth() + "*" + mPreviewSize.getHeight());
            //mPreviewSize = streamConfigurationMap.getOutputSizes(SurfaceTexture.class)[2];
            Log.d(TAG, "preview size: " + mPreviewSize.getWidth() + "*" + mPreviewSize.getHeight());
            mImageReader = ImageReader.newInstance(mPreviewSize.getWidth(),mPreviewSize.getHeight(),ImageFormat.JPEG,2);
            Log.d(TAG, "preview size: " + mPreviewSize.getWidth() + "*" + mPreviewSize.getHeight());
            //打开camera
                if (!mCameraOpenCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                    throw new RuntimeException("Time out waiting to lock camera opening.");
                }

            mCameraManager.openCamera(Integer.toString(mCameraId), mStateCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }catch (InterruptedException e){
            throw new RuntimeException("Interrupted while trying to lock camera opening.", e);
        }
    }

    public void releaseCamera(){
        //关闭CameraCaptureSession
        try {
            mCameraOpenCloseLock.acquire();
            if (mCameraCaptureSession != null) {
                mCameraCaptureSession.close();
                mCameraCaptureSession = null;
            }
            //关闭CameraDevice
            if (mCameraDevice != null) {
                mCameraDevice.close();
                mCameraDevice = null;
            }
            //关闭图片预览
            if (mImageReader != null) {
                mImageReader.close();
                mImageReader = null;
            }
            mScreenOrientationDetector.disable();
            //mOrientationEventListener.disable();
            //关闭后台线程
            stopBackgroundThread();
        }catch (InterruptedException e){
            throw new RuntimeException("Interrupted while trying to lock camera closing.", e);
        }finally {
            mCameraOpenCloseLock.release();
        }
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
        if (mCameraCaptureSession == null || mPreviewRequestBuilder == null) {
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
        if (mCameraCaptureSession == null || mPreviewRequestBuilder == null) {
            Log.w(TAG, "startPreview: mCaptureSession or mPreviewRequestBuilder is null");
            return;
        }
        try {
            mCameraCaptureSession.setRepeatingRequest(mPreviewRequest, null, mBackgroundHandler);
        }catch (CameraAccessException e){
            e.printStackTrace();
        }
    }

    public void switchCamera(int width,int height){
        mCameraId ^= 1;
        Log.d(TAG, "switchCamera: mCameraId: " + mCameraId);
        releaseCamera();
        openCamera(width,height);
    }

    private CameraCaptureSession.CaptureCallback mCaptureCallback //= null;
            = new CameraCaptureSession.CaptureCallback() {

        private void process(CaptureResult result) {
            switch (mState) {
                case CameraState.STATE_PREVIEW: {
                    // We have nothing to do when the camera preview is working normally.
                    break;
                }
                case CameraState.STATE_WAITING_LOCK: {
                    Integer afState = result.get(CaptureResult.CONTROL_AF_STATE);
                    if (afState == null) {
                        captureStillPicture();
                    } else if (CaptureResult.CONTROL_AF_STATE_FOCUSED_LOCKED == afState ||
                            CaptureResult.CONTROL_AF_STATE_NOT_FOCUSED_LOCKED == afState) {
                        // CONTROL_AE_STATE can be null on some devices
                        Integer aeState = result.get(CaptureResult.CONTROL_AE_STATE);
                        if (aeState == null ||
                                aeState == CaptureResult.CONTROL_AE_STATE_CONVERGED) {
                            mState = CameraState.STATE_PICTURE_TAKEN;
                            captureStillPicture();
                        } else {
                            runPrecaptureSequence();
                        }
                    }
                    break;
                }
                case CameraState.STATE_WAITING_PRECAPTURE: {
                    // CONTROL_AE_STATE can be null on some devices
                    Integer aeState = result.get(CaptureResult.CONTROL_AE_STATE);
                    if (aeState == null ||
                            aeState == CaptureResult.CONTROL_AE_STATE_PRECAPTURE ||
                            aeState == CaptureRequest.CONTROL_AE_STATE_FLASH_REQUIRED) {
                        mState = CameraState.STATE_WAITING_NON_PRECAPTURE;
                    }
                    break;
                }
                case CameraState.STATE_WAITING_NON_PRECAPTURE: {
                    // CONTROL_AE_STATE can be null on some devices
                    Integer aeState = result.get(CaptureResult.CONTROL_AE_STATE);
                    if (aeState == null || aeState != CaptureResult.CONTROL_AE_STATE_PRECAPTURE) {
                        mState = CameraState.STATE_PICTURE_TAKEN;
                        captureStillPicture();
                    }
                    break;
                }
            }
        }
        @Override
        public void onCaptureProgressed(@NonNull CameraCaptureSession session,
                                        @NonNull CaptureRequest request,
                                        @NonNull CaptureResult partialResult) {
            process(partialResult);
        }

        @Override
        public void onCaptureCompleted(@NonNull CameraCaptureSession session,
                                       @NonNull CaptureRequest request,
                                       @NonNull TotalCaptureResult result) {
            process(result);
        }

        @Override
        public void onCaptureSequenceAborted( @NonNull CameraCaptureSession session,
                                             int sequenceId) {
            // default empty implementation
            Log.i(TAG,"onCaptureSequenceAborted"+sequenceId);
        }
    };
    private void runPrecaptureSequence() {
        try {
            // This is how to tell the camera to trigger.
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_PRECAPTURE_TRIGGER,
                    CaptureRequest.CONTROL_AE_PRECAPTURE_TRIGGER_START);
            // Tell #mCaptureCallback to wait for the precapture sequence to be set.
            mState = CameraState.STATE_WAITING_PRECAPTURE;
            mCameraCaptureSession.capture(mPreviewRequestBuilder.build(), null,
                    mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }


    public void initPreviewRequest() {
        try {
            mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            if(mPerviewSurface==null&&mPreviewSurfaceTexture!=null){
                mPreviewSurfaceTexture.setDefaultBufferSize(mPreviewSize.getWidth(),mPreviewSize.getHeight());
                mPerviewSurface = new Surface(mPreviewSurfaceTexture);
            }
            mPreviewRequestBuilder.addTarget(mPerviewSurface);
            mCameraDevice.createCaptureSession(Arrays.asList(mPerviewSurface, mImageReader.getSurface()),
                    new CameraCaptureSession.StateCallback() {
                        @Override
                        public void onConfigured(@NonNull CameraCaptureSession session) {
                            mCameraCaptureSession = session;
                            // 设置连续自动对焦
                            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest
                                    .CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                            // 设置自动曝光
                            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest
                                    .CONTROL_AE_MODE_ON_AUTO_FLASH);
                            // 设置完后自动开始预览
                            mPreviewRequest = mPreviewRequestBuilder.build();
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

    private Size chooseOptimalSize(Size[] sizes,int viewWidth, int viewHeight){
        // 1. 确定期望的宽高
        int desiredWidth;
        int desiredHeight;
        if (mScreenOrientationDetector.isLandscape()) {// 横屏状态, 直接使用 view 的宽高
            desiredWidth = viewWidth;
            desiredHeight = viewHeight;
        } else {// 竖屏状态, 反转宽高
            desiredWidth = viewHeight;
            desiredHeight = viewWidth;
        }
        Log.v(TAG,"desiredWidth: "+desiredWidth+"desiredHeight: "+desiredHeight);
        Size result = null;
        for (Size size : sizes) {// sizes 已按照从小到大排序
            result = size;
            // 选取宽高都比期望稍大的
            if (desiredWidth <= size.getWidth() && desiredHeight <= size.getHeight()) {
                return size;
            }
        }
        //return new Size(1920,1080);
        return result;
    }

    public Size getPreviewSize(){
        return mPreviewSize;
    }

    public void captureStillPicture(){
        try{
            CaptureRequest.Builder captureBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);
            captureBuilder.addTarget(mImageReader.getSurface());
            captureBuilder.set(CaptureRequest.CONTROL_AE_MODE,CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);//自动曝光？
            captureBuilder.set(CaptureRequest.JPEG_ORIENTATION,mScreenOrientationDetector.getOrientation(
                    mCameraCharacteristics.get(CameraCharacteristics.SENSOR_ORIENTATION)));//旋转角度
            //TODO：放大
            mCameraCaptureSession.stopRepeating();//停止任何一个正常进行的重复请求
            //mCameraCaptureSession.abortCaptures();//尽可能快的取消当前队列中或正在处理中的所有捕捉请求
            final long time = System.currentTimeMillis();
            mCameraCaptureSession.capture(captureBuilder.build(), new CameraCaptureSession.CaptureCallback() {
                @Override
                public void onCaptureStarted(CameraCaptureSession session, CaptureRequest request, long timestamp, long frameNumber) {
                    // default empty implementation
                    Log.d(TAG, "onCaptureStarted, time: " + (System.currentTimeMillis() - time));
                    Log.d(TAG, "onCaptureStarted, time: " + timestamp);

                }
                @Override
                public void onCaptureCompleted(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull TotalCaptureResult result) {
                    Log.d(TAG, "onCaptureCompleted, time: " + (System.currentTimeMillis() - time));
                    try{
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_CANCEL);
                        //mPreviewRequest = mPreviewRequestBuilder.build();
                        mCameraCaptureSession.capture(mPreviewRequestBuilder.build(),null,mBackgroundHandler);
                    } catch (CameraAccessException e) {
                        e.printStackTrace();
                    }
                    startPreview();
                }
            }, mBackgroundHandler);
        } catch (CameraAccessException e) {
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

    public boolean isFront(){
        return mCameraId == 1;
    }
    public int getOrientation(){
        return ScreenOrientationDetector.DISPLAY_ORIENTATIONS.get(mScreenOrientationDetector.mLastRotation);
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
}

