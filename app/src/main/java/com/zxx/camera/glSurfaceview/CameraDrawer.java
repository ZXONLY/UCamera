package com.zxx.camera.glSurfaceview;

import android.opengl.GLES20;
import android.opengl.GLES30;

import com.zxx.camera.Utils.OpenGLutil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class CameraDrawer {
    private final String vertexShaderCode =
            "#version 300 es\n" +
            "\n" +
            "layout (location = 0) in vec2 aPos;\n" +
            "layout (location = 2) in vec2 aTexCoord;\n" +
            "\n" +
            "out vec2 TexCoord；\n" +
            "\n" +
            "void main(){\n" +
            "gl_Position = vec4(aPos,0.0,1.0);\n" +
            "TexCoord = aTexCoord;\n" +
            "}";
    private final String fragmentShaderCode =
            "#version 300 es\n" +
            "out vec4 FragColor;\n" +
            "\n" +
            "in vec2 TexCoord;\n" +
            "\n" +
            "uniform sampler2D ourTexture;\n" +
            "\n" +
            "void main()\n" +
            "{\n" +
            "    FragColor = texture(ourTexture, TexCoord);\n" +
            "}";

    static float squareCoords[] = {
            -1.0f,-1.0f,
            -1.0f,1.0f,
            1.0f,1.0f,
            1.0f,-1.0f
    };
    static float textureVertices[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
    };
    private FloatBuffer vertexBuffer,textureVerticesBuffer;
    private ByteBuffer mDrawListBuffer;
    private final int mProgram;
    private int texture;

    private static final byte VERTEX_ORDER[] = { 0, 1, 2, 3 }; // order to draw vertices

    public CameraDrawer(){
        vertexBuffer = ByteBuffer.allocateDirect(squareCoords.length*4).order(ByteOrder.nativeOrder()).asFloatBuffer().put(squareCoords);
        textureVerticesBuffer = ByteBuffer.allocateDirect(textureVertices.length*4).order(ByteOrder.nativeOrder()).asFloatBuffer().put(textureVertices);
        mDrawListBuffer = ByteBuffer.allocateDirect(VERTEX_ORDER.length).order(ByteOrder.nativeOrder());
        mDrawListBuffer.put(VERTEX_ORDER).position(0);

        mProgram = OpenGLutil.createProgram(vertexShaderCode,fragmentShaderCode);
    }

    public void setTexture(int texture){
        this.texture = texture;
    }

    public void draw(){
        GLES30.glUseProgram(mProgram);

        //激活并绑定OES纹理
        GLES30.glActiveTexture(GLES30.GL_TEXTURE0);
        GLES30.glBindTexture(GLES30.GL_TEXTURE0,texture);

        int mPositionHandle = GLES30.glGetAttribLocation(mProgram,"aPos");
        GLES30.glEnableVertexAttribArray(mPositionHandle);
        GLES30.glVertexAttribIPointer(mPositionHandle,2,GLES30.GL_FLOAT,0,vertexBuffer);

        int mTextureCoordHandle = GLES30.glGetAttribLocation(mProgram, "aTexCoord");
        GLES30.glEnableVertexAttribArray(mTextureCoordHandle);
        GLES30.glVertexAttribIPointer(mPositionHandle,2,GLES30.GL_FLOAT,0,textureVerticesBuffer);


        GLES20.glDrawElements(GLES30.GL_TRIANGLE_FAN, VERTEX_ORDER.length, GLES30.GL_UNSIGNED_BYTE, mDrawListBuffer);

        GLES30.glDisableVertexAttribArray(mPositionHandle);
        GLES30.glDisableVertexAttribArray(mTextureCoordHandle);
    }

//openGL 搭建一个EGL，创建一个线程（先在java，再用native）
}
