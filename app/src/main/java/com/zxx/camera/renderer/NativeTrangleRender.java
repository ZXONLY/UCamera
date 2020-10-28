package com.zxx.camera.renderer;

import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class NativeTrangleRender implements GLSurfaceView.Renderer {

    //顶点着色器
    private final String verticesShaderSource =
            "#version 300 es\n" +
                    "layout (location = 0) in vec4 aPosition;\n" +
                    "layout (location = 1) in vec4 aColor;\n" +
                    "out vec4 vColor;\n" +
                    "void main() {\n" +
                    "     gl_Position  = aPosition;\n" +
                    //"     gl_PointSize = 10.0;\n" +
                    "     vColor = aColor;\n" +
                    "}";

    private final String FRAGMENT_SHADER =
            "#version 300 es\n" +
                    "precision mediump float;\n" +
                    "in vec4 vColor;\n" +
                    "out vec4 fragColor;\n" +
                    "void main() {\n" +
                    "     fragColor = vColor;\n" +
                    "}                                     \n";
    static {
        System.loadLibrary("native-lib");
    }
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.d("hhh","onSurfaceCreated");
        nativeInit(verticesShaderSource, FRAGMENT_SHADER);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.d("hhh","onSurfaceChanged");
        nativeDraw();
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        Log.d("hhh","onDrawFrame");
        nativeDraw();
    }

    public static native void nativeInit(String vertexShaderCode, String fragmentShaderCode);
    private static native void nativeDraw();
}
