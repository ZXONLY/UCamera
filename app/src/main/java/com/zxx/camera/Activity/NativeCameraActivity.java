package com.zxx.camera.Activity;

import android.os.Bundle;
import android.view.SurfaceView;

import androidx.appcompat.app.AppCompatActivity;

import com.zxx.camera.R;
import com.zxx.camera.renderer.NativeCameraRender;

public class NativeCameraActivity extends AppCompatActivity {
    private final static String TAG = "NativeTranglesActivity";
    private SurfaceView mSurfaceView;
    private NativeCameraRender cameraRender;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_native_camera);
        mSurfaceView = findViewById(R.id.native_camera);
        cameraRender = new NativeCameraRender();
        cameraRender.init(this,mSurfaceView.getWidth(),mSurfaceView.getHeight());
        mSurfaceView.getHolder().addCallback(cameraRender);
    }
}
