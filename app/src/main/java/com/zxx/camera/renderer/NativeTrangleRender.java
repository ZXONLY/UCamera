package com.zxx.camera.renderer;

import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.Surface;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class NativeTrangleRender //implements GLSurfaceView.Renderer
{
    static {
        Log.d("test","load avcodec");
        System.loadLibrary("avcodec");
        Log.d("test","load avfilter");
        System.loadLibrary("avfilter");
        Log.d("test","load avformat");
        System.loadLibrary("avformat");
        Log.d("test","load avutil");
        System.loadLibrary("avutil");
        Log.d("test","load swresample");
        System.loadLibrary("swresample");
        Log.d("test","load swscale");
        System.loadLibrary("swscale");
        Log.d("test","load native");
        System.loadLibrary("native-lib");
    }
    //顶点着色器
    private final String verticesShaderSource =
            "#version 300 es\n" +
                    "uniform mat4 uMVPMatrix;"+
                    "layout (location = 0) in vec4 aPosition;\n" +
                    "layout (location = 1) in vec4 aColor;\n" +
                    "out vec4 vColor;\n" +
                    "void main() {\n" +
                    "     gl_Position  = uMVPMatrix * aPosition;\n" +
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
    //@Override
    public void onSurfaceCreated(Surface surface) {
        Log.d("hhh","onSurfaceCreated");
        nativeInit(surface,verticesShaderSource, FRAGMENT_SHADER);
    }

    //@Override
    public void onSurfaceChanged( int width, int height) {
        Log.d("hhh","onSurfaceChanged");
        nativeSurfaceChanged(width,height);
    }

    //@Override
    public void onDrawFrame(GL10 gl) {
        Log.d("hhh","onDrawFrame");
        nativeDraw();
    }

    public void onSurfaceDestroy(){
        nativeStopEGLThread();
    }

    public static native void nativeInit(Surface surface,String vertexShaderCode, String fragmentShaderCode);
    private static native void nativeDraw();
    private static native void nativeSurfaceChanged(int width, int height);
    private static native void nativeStopEGLThread();
}
