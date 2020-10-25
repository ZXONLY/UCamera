package com.zxx.camera.renderer;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class TextureRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = "TextureRenderer";

    private final String VERTEX_SHADER =
            "#version 300 es\n" +
                    "in vec4 vPosition;\n"+
                    "in vec2 a_texCoord;\n"+
                    "out vec2 v_texCoord;\n"+
                    "void main() {\n" +
                    "     gl_Position  = vPosition;\n" +
                    "     gl_PointSize = 10.0;\n" +
                    "     vColor = aColor;\n" +
                    "}";



    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

    }

    @Override
    public void onDrawFrame(GL10 gl) {

    }
}
