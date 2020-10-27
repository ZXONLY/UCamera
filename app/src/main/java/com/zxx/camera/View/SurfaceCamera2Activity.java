package com.zxx.camera.View;

import android.content.Intent;
import android.media.ImageReader;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.view.View;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;

import com.zxx.camera.Camera.camera2Proxy;
import com.zxx.camera.R;
import com.zxx.camera.Utils.ImageUtils;

import java.io.File;
import java.text.SimpleDateFormat;

public class SurfaceCamera2Activity extends AppCompatActivity implements View.OnClickListener {

    private static final String TAG = "SurfaceCameraActivity";

    private ImageView mCloseIv;
    private ImageView mSwitchCameraIv;
    private ImageView mTakePictureIv;
    private ImageView mPictureIv;
    private Camera2SurfaceView mCameraView;
    private camera2Proxy mCameraProxy;
    private ImageUtils mImageUtils;

    private static final String GALLERY_PATH = Environment.getExternalStoragePublicDirectory(Environment
            .DIRECTORY_DCIM) + File.separator + "Camera";
    private static final SimpleDateFormat DATE_FORMAT = new SimpleDateFormat("yyyyMMdd_HHmmss");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_glsurfacecamera2);
        mCloseIv = findViewById(R.id.toolbar_close_iv);
        mCloseIv.setOnClickListener(this);
        mSwitchCameraIv = findViewById(R.id.toolbar_switch_iv);
        mSwitchCameraIv.setOnClickListener(this);
        //mPictureIv = findViewById(R.id.picture_iv);
        //mPictureIv.setImageBitmap(getLatestThumbBitmap());
        mTakePictureIv = findViewById(R.id.take_picture_iv);
        mTakePictureIv.setOnClickListener(this);
        mCameraView = findViewById(R.id.camera_view);
        mCameraProxy = mCameraView.getCameraProxy();
        mImageUtils = ImageUtils.getInstance();
        mImageUtils.init(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.toolbar_close_iv:
                finish();
                break;
            case R.id.toolbar_switch_iv:
                mCameraProxy.switchCamera(mCameraView.getWidth(), mCameraView.getHeight());
                mCameraProxy.startPreview();
                break;
            case R.id.take_picture_iv:
                mCameraProxy.setImageAvailableListener(mOnImageAvailableListener);
                mCameraProxy.captureStillPicture(); // 拍照
                //mCameraProxy.takePicture();
                break;
            case R.id.picture_iv:
                Intent intent = new Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                startActivity(intent);
                break;
        }
    }
    public final ImageReader.OnImageAvailableListener mOnImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            mImageUtils.saveImage(reader.acquireNextImage());
        }
    };
}
