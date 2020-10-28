package com.zxx.camera.View;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class NativeTrangleSurfaceView extends GLSurfaceView {
    public NativeTrangleSurfaceView(Context context, GLSurfaceView.Renderer renderer) {
        super(context);
        this.setEGLContextClientVersion(3);
        this.setRenderer(renderer);
        this.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }
}
