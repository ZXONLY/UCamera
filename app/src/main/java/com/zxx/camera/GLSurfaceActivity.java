package com.zxx.camera;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.Image;
import android.media.ImageReader;
import android.os.AsyncTask;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;

import com.zxx.camera.Camera.camera2Proxy;
import com.zxx.camera.glSurfaceview.Camera2GLSurfaceView;

import java.nio.ByteBuffer;

public class GLSurfaceActivity extends AppCompatActivity {
    private static final String TAG = "GLSurfaceCamera2Act";

    private ImageView mCloseIv;
    private ImageView mSwitchCameraIv;
    private ImageView mTakePictureIv;
    private ImageView mPictureIv;
    private Camera2GLSurfaceView mCameraView;

    private camera2Proxy mCameraProxy;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_glsurface_camera2);
        initView();
    }

    private void initView() {
        mCloseIv = findViewById(R.id.toolbar_close_iv);

        mSwitchCameraIv = findViewById(R.id.toolbar_switch_iv);

        mTakePictureIv = findViewById(R.id.take_picture_iv);

        mPictureIv = findViewById(R.id.picture_iv);

        mCameraView = findViewById(R.id.camera_view_gl);
        mCameraProxy = mCameraView.getCameraProxy();
    }



}

