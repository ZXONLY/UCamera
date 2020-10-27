package com.zxx.camera;

import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.zxx.camera.View.EglSurfaceView;
import com.zxx.camera.renderer.EGLRender;

public class newEGLCamera2Activity extends AppCompatActivity {
    private static String TAG = "EglSurfaceActivity";

    private EGLRender eglRenderer;
    private EglSurfaceView eglSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_new_eglsurface);
        eglSurfaceView = findViewById(R.id.new_egl_surface);
        eglSurfaceView.getHandler();
        eglRenderer = new EGLRender();
        eglSurfaceView.setRender(eglRenderer);
        eglRenderer.init(this,eglSurfaceView.getWidth(),eglSurfaceView.getHeight());
    }
}
