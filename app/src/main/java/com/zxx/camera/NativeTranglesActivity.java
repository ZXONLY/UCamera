package com.zxx.camera;

import android.opengl.GLSurfaceView;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.zxx.camera.View.NativeTrangleSurfaceView;
import com.zxx.camera.renderer.NativeTrangleRender;

public class NativeTranglesActivity extends AppCompatActivity {
    private final static String TAG = "NativeTranglesActivity";

    private NativeTrangleRender mNativeTrangleRender;
    private GLSurfaceView mGLView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mNativeTrangleRender = new NativeTrangleRender();
        mGLView = new NativeTrangleSurfaceView(this,mNativeTrangleRender);
        setContentView(mGLView);
    }
}
