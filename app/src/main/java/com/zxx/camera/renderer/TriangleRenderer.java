package com.zxx.camera.renderer;

import android.opengl.GLES30;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.util.Log;

import com.zxx.camera.Utils.OpenGLutil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class TriangleRenderer {//implements GLSurfaceView.Renderer {

    //顶点着色器
    private final String verticesShaderSource =
                    "#version 300 es\n" +
                    "layout (location = 0) in vec4 vPosition;\n" +
                    "layout (location = 1) in vec4 aColor;\n" +
                    "out vec4 vColor;\n" +
                    "void main() {\n" +
                    "     gl_Position  = vPosition;\n" +
                    "     gl_PointSize = 10.0;\n" +
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
    //顶点坐标
    private static final float[] vertices = {
            0.0f, 0.5f,0.0f,1.0f,
            -0.5f, -0.5f,0.0f,1.0f,
            0.5f, -0.5f,0.0f,1.0f
    };
    private float color[] = {
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f
    };
    private FloatBuffer colorBuffer;
    private FloatBuffer verticesBuffer;

    public TriangleRenderer(){
        //将顶点数据拷贝映射到 native 内存中，以便opengl能够访问
        mProgram = OpenGLutil.createProgram(verticesShaderSource,FRAGMENT_SHADER);
        verticesBuffer = ByteBuffer
                .allocateDirect(vertices.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertices);
        verticesBuffer.position(0);

        colorBuffer = ByteBuffer
                .allocateDirect(color.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(color);
        colorBuffer.position(1);
    }

    private int mProgram;
//    @Override
//    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
//        mProgram = OpenGLutil.createProgram(verticesShaderSource,FRAGMENT_SHADER);
//        //if(mProgram == GLES30.GL_NONE)
//        GLES30.glClearColor(1.0f,1.0f,1.0f,0.0f);
//    }
//
//    @Override
//    public void onSurfaceChanged(GL10 gl, int width, int height) {
//        GLES30.glViewport(0,0,width,height);
//    }
//
//    @Override
//    public void onDrawFrame(GL10 gl) {
//        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);
//
//        GLES30.glUseProgram(mProgram);
//        //准备坐标数据
//        int vposition = GLES30.glGetAttribLocation(mProgram, "vPosition");
//        GLES30.glVertexAttribPointer(vposition, 4, GLES30.GL_FLOAT, false, 0, verticesBuffer);
//        //启用顶点的句柄
//        GLES30.glEnableVertexAttribArray(vposition);
//
//        //绘制三角形颜色
//        int cloroposition = GLES30.glGetAttribLocation(mProgram, "aColor");
//        GLES30.glEnableVertexAttribArray(cloroposition);
//        GLES30.glVertexAttribPointer(cloroposition, 4, GLES30.GL_FLOAT, false, 0, colorBuffer);
//
//        GLES30.glDrawArrays(GLES30.GL_TRIANGLES, 0, 3);
//
//        //禁止顶点数组的句柄
//        GLES30.glDisableVertexAttribArray(vposition);
//        GLES30.glDisableVertexAttribArray(cloroposition);
//    }

    public void drawTriangle(){
        GLES30.glClearColor(1.0f,1.0f,1.0f,0.0f);
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);

        GLES30.glUseProgram(mProgram);
        //准备坐标数据
        int vposition = GLES30.glGetAttribLocation(mProgram, "vPosition");
        GLES30.glVertexAttribPointer(vposition, 4, GLES30.GL_FLOAT, false, 0, verticesBuffer);
        //启用顶点的句柄
        GLES30.glEnableVertexAttribArray(vposition);

        //绘制三角形颜色
        int cloroposition = GLES30.glGetAttribLocation(mProgram, "aColor");
        GLES30.glEnableVertexAttribArray(cloroposition);
        GLES30.glVertexAttribPointer(cloroposition, 4, GLES30.GL_FLOAT, false, 0, colorBuffer);

        GLES30.glDrawArrays(GLES30.GL_TRIANGLES, 0, 3);

        //禁止顶点数组的句柄
        GLES30.glDisableVertexAttribArray(vposition);
        GLES30.glDisableVertexAttribArray(cloroposition);
    }
}