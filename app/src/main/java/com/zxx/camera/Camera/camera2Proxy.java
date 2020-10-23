package com.zxx.camera.Camera;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.Activity;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.Rect;
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
import android.hardware.camera2.params.MeteringRectangle;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.provider.MediaStore;
import android.util.Log;
import android.util.Size;
import android.view.Display;
import android.view.OrientationEventListener;
import android.view.Surface;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;
import androidx.core.os.EnvironmentCompat;

import com.zxx.camera.Utils.ImageUtils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;

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

    private static final String GALLERY_PATH = Environment.getExternalStoragePublicDirectory(Environment
            .DIRECTORY_DCIM) + File.separator + "Camera";
    private static final SimpleDateFormat DATE_FORMAT = new SimpleDateFormat("yyyyMMdd_HHmmss");


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
        //mOrientationEventListener.enable();
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
            mCameraManager.openCamera(Integer.toString(mCameraId), mstateCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    public void releaseCamera(){
        //关闭CameraCaptureSession
        if(mCameraCaptureSession!=null) {
            mCameraCaptureSession.close();
            mCameraCaptureSession = null;
        }
        //关闭CameraDevice
        if(mCameraDevice!=null) {
            mCameraDevice.close();
            mCameraDevice = null;
        }
        //关闭图片预览
        if(mImageReader!=null) {
            mImageReader.close();
            mImageReader = null;
        }
        mScreenOrientationDetector.disable();
        //mOrientationEventListener.disable();
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

    public void switchCamera(int width,int height){
        //TODO
        mCameraId ^= 1;
        Log.d(TAG, "switchCamera: mCameraId: " + mCameraId);
        releaseCamera();
        openCamera(width,height);
    }


    public void initPreviewRequest() {
        try {
            mCaptureRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            if(mPerviewSurface==null&&mPreviewSurfaceTexture!=null){
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
            mCameraCaptureSession.abortCaptures();//尽可能快的取消当前队列中或正在处理中的所有捕捉请求
            final long time = System.currentTimeMillis();
            mCameraCaptureSession.capture(captureBuilder.build(), new CameraCaptureSession.CaptureCallback() {
                @Override
                public void onCaptureCompleted(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull TotalCaptureResult result) {
                    Log.w(TAG, "onCaptureCompleted, time: " + (System.currentTimeMillis() - time));
                    try{
                        mCaptureRequestBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER, CameraMetadata.CONTROL_AF_TRIGGER_CANCEL);
                        mCaptureRequest = mCaptureRequestBuilder.build();
                        mCameraCaptureSession.capture(mCaptureRequest,null,mBackgroundHandler);
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
}
