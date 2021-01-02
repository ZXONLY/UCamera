package com.zxx.camera.renderer;

import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLES30;
import android.opengl.Matrix;
import android.util.Log;

import com.zxx.camera.Utils.OpenGLutil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class CameraRender {
    private final String vertexShaderCode = "" +
            "#version 300 es\n" +
            "uniform mat4 uMVPMatrix;" +
            "layout (location = 0) in vec4 vPosition;\n" +
            "layout (location = 1) in vec2 aTextureCoord;\n" +
            "out vec2 yuvTexCoords;\n" +
            "void main() { \n" +
            "     gl_Position  = uMVPMatrix * vPosition;\n" +
            "     gl_PointSize = 10.0;\n" +
            "     yuvTexCoords = aTextureCoord;\n" +
            "}";
    private final String fragmentShaderCode =
            "#version 300 es\n" +
            "#extension GL_OES_EGL_image_external_essl3 : require\n" +
            "precision mediump float;\n" +
            "uniform samplerExternalOES yuvTexSampler;\n" +
            "in vec2 yuvTexCoords;\n" +
            "out vec4 vFragColor;\n" +
            "void main() {\n" +
            "     vFragColor = texture(yuvTexSampler,yuvTexCoords);\n" +
            "}";

    static float squareCoords[] = {//顶点坐标
            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f, 1.0f,
            -1.0f,  1.0f,
    };
    static float textureVertices[] = {//纹理坐标V0
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f,1.0f
    };
    private FloatBuffer vertexBuffer,textureVerticesBuffer;
    private ByteBuffer mDrawListBuffer;
    private final int mProgram;
    private int texture;
    private int mPositionHandle;
    private int mTextureHandle;
    private int uniformSamplers;
    private int mMatrixHandle;

    private static final byte VERTEX_ORDER[] = { 0, 1, 2, 3 }; // order to draw vertices

    private float [] mMVPMatrix = new float[16] ;

//    private static final byte VERTEX_ORDER[] = {
//            0, 1, 2,  //V0,V1,V2 三个顶点组成一个三角形
//            0, 2, 3,  //V0,V2,V3 三个顶点组成一个三角形
//            0, 3, 4,  //V0,V3,V4 三个顶点组成一个三角形
//            0, 4, 1   //V0,V4,V1 三个顶点组成一个三角形
//     }; // order to draw vertices

    public CameraRender(){

        vertexBuffer = ByteBuffer.allocateDirect(squareCoords.length*4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        vertexBuffer.put(squareCoords).position(0);

        textureVerticesBuffer = ByteBuffer.allocateDirect(textureVertices.length*4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        textureVerticesBuffer.put(textureVertices).position(0);

        mDrawListBuffer = ByteBuffer.allocateDirect(VERTEX_ORDER.length).order(ByteOrder.nativeOrder());
        mDrawListBuffer.put(VERTEX_ORDER).position(0);

        mProgram = OpenGLutil.createProgram(vertexShaderCode,fragmentShaderCode);

    }

    public void setTexture(int texture){
        this.texture = texture;
    }

    public void draw(boolean isFront,int dataWidth,int dataHeight,int width,int height,int Orientation){
        GLES30.glUseProgram(mProgram);
        mPositionHandle = GLES30.glGetAttribLocation(mProgram, "vPosition");
        mTextureHandle = GLES30.glGetAttribLocation(mProgram, "aTextureCoord");
        uniformSamplers = GLES30.glGetUniformLocation(mProgram,"yuvTexSampler");
        mMatrixHandle = GLES30.glGetUniformLocation(mProgram, "uMVPMatrix");

        mMVPMatrix = OpenGLutil.getShowMatrix(dataWidth,dataHeight,width,height);
        if(isFront){
            OpenGLutil.flip(mMVPMatrix,true,false);
            OpenGLutil.rotate(mMVPMatrix,(180-Orientation+360)%360);
        }else {
            OpenGLutil.rotate(mMVPMatrix,(Orientation)%360);
        }
        GLES20.glUniformMatrix4fv(mMatrixHandle, 1, false, mMVPMatrix, 0);
        //激活并绑定OES纹理
        GLES30.glActiveTexture(GLES30.GL_TEXTURE0);
        GLES30.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texture);
        GLES30.glUniform1i(uniformSamplers, 0);

        GLES30.glEnableVertexAttribArray(mPositionHandle);
        GLES30.glVertexAttribPointer(mPositionHandle,2,GLES30.GL_FLOAT,false,8,vertexBuffer);


        GLES30.glEnableVertexAttribArray(mTextureHandle);
        GLES30.glVertexAttribPointer(mTextureHandle, 2, GLES30.GL_FLOAT, false, 8, textureVerticesBuffer);
        //Log.i("hhh","2");
        GLES30.glDrawElements(GLES30.GL_TRIANGLE_FAN,4,GLES30.GL_UNSIGNED_BYTE,mDrawListBuffer);
        //Log.i("hhh","3");
        GLES30.glDisableVertexAttribArray(mPositionHandle);
        GLES30.glDisableVertexAttribArray(mTextureHandle);
    }
}

