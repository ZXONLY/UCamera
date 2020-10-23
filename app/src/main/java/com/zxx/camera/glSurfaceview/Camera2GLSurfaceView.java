package com.zxx.camera.glSurfaceview;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class Camera2GLSurfaceView extends GLSurfaceView {


    public Camera2GLSurfaceView(Context context) {
        this(context,null);
    }
    public Camera2GLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    void init(Context context){

    }

}
