package com.zxx.unique.mediacodec;

import android.opengl.GLES30;
import android.util.Log;

import com.zxx.camera.Utils.OpenGLutil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class YuvTextureDrawer {

    private static final String TAG = YuvTextureDrawer.class.getSimpleName();

    private static final String VERTEX_SHADER_CODE = "" +
            "#version 300 es\n" +
            "layout (location = 0) in vec4 av_Position;\n" +
            "layout (location = 1) in vec2 af_Position;\n" +
            "out vec2 v_texPo;\n" +
            "void main() {\n" +
            "gl_Position = av_Position;\n" +
            "v_texPo = af_Position;\n" +
            "}";
    private static final String FRAGMENT_SHADER_CODE = "" +
            "#version 300 es\n" +
            "precision mediump float;\n" +
            "in vec2 v_texPo;\n" +
            "uniform sampler2D sampler_y;\n" +
            "uniform sampler2D sampler_u;\n" +
            "uniform sampler2D sampler_v;\n" +
            "void main() {\n" +
            "float y, u, v;\n" +
            "vec3 rgb;\n" +
            "y = texture2D(sampler_y, v_texPo).r;\n" +
            "u = texture2D(sampler_u, v_texPo).r - 0.5;\n" +
            "v = texture2D(sampler_v, v_texPo).r - 0.5;\n" +
            "rgb.r = y + 1.403 * v;\n" +
            "rgb.g = y - 0.344 * u - 0.714 * v;\n" +
            "rgb.b = y + 1.770 * u;\n" +
            "gl_FragColor = vec4(rgb, 1);\n" +
            "}";

    private static final float[] vertexData = {
            -1f, -1f, 0f,
            1f, -1f, 0f,
            -1, 1f, 0f,
            1f, 1, 0f,
    };
    private static final float[] textureData = {
            0f, 1f, 0f,
            1f, 1f, 0f,
            0f, 0f, 0f,
            1f, 0f, 0f,
    };

    private static final int COORDS_PER_TEX = 3;
    private static final int VERTEX_COUNT = vertexData.length / COORDS_PER_TEX;
    private static final int VERTEX_STRIDE = COORDS_PER_TEX * 4;

    private FloatBuffer vertexBuffer;
    private FloatBuffer textureBuffer;

    private int mProgram;
    private int avPosition;
    private int afPosition;

    private int[] vboIds;

    private int sampler_y;
    private int sampler_u;
    private int sampler_v;
    private int[] textureId_yuv;

    private int width_yuv;
    private int height_yuv;
    private ByteBuffer y;
    private ByteBuffer u;
    private ByteBuffer v;

    public YuvTextureDrawer() {
        vertexBuffer = ByteBuffer.allocateDirect(vertexData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertexData);
        vertexBuffer.position(0);

        textureBuffer = ByteBuffer.allocateDirect(textureData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(textureData);
        textureBuffer.position(0);
    }

    public void setYUVData(int width, int height, byte[] yuv) {
        this.width_yuv = width;
        this.height_yuv = height;
        byte[] yData = new byte[width * height];
        byte[] uData = new byte[width * height / 4];
        byte[] vData = new byte[width * height / 4];
        extractYUV(yuv, yData, uData, vData);
        this.y = ByteBuffer.allocateDirect(yData.length)
                .order(ByteOrder.nativeOrder())
                .put(yData);
        y.position(0);
        this.u = ByteBuffer.allocateDirect(uData.length)
                .order(ByteOrder.nativeOrder())
                .put(uData);
        u.position(0);
        this.v = ByteBuffer.allocateDirect(vData.length)
                .order(ByteOrder.nativeOrder())
                .put(vData);
        v.position(0);
    }

    public int init() {
        mProgram = OpenGLutil.createProgram(VERTEX_SHADER_CODE, FRAGMENT_SHADER_CODE);
        if (mProgram == 0) {
            Log.e(TAG, "Create program failed.");
            return -1;
        }
        avPosition = GLES30.glGetAttribLocation(mProgram, "av_Position");
        afPosition = GLES30.glGetAttribLocation(mProgram, "af_Position");

        sampler_y = GLES30.glGetUniformLocation(mProgram, "sampler_y");
        sampler_u = GLES30.glGetUniformLocation(mProgram, "sampler_u");
        sampler_v = GLES30.glGetUniformLocation(mProgram, "sampler_v");

        textureId_yuv = new int[3];
        GLES30.glGenTextures(textureId_yuv.length, textureId_yuv, 0);

        for (int id : textureId_yuv) {
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, id);
            GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_REPEAT);
            GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_REPEAT);
            GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_LINEAR);
            GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_LINEAR);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, 0);
        }

        vboIds = new int[1];
        GLES30.glGenBuffers(vboIds.length, vboIds, 0);
        int ret = checkGLError("glGenBuffers");
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboIds[0]);
        GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, vertexData.length * 4 + textureData.length * 4, null, GLES30.GL_STATIC_DRAW);

        GLES30.glBufferSubData(GLES30.GL_ARRAY_BUFFER, 0, vertexData.length * 4, vertexBuffer);
        GLES30.glBufferSubData(GLES30.GL_ARRAY_BUFFER, vertexData.length * 4, textureData.length * 4, textureBuffer);
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, 0);

        return ret;
    }

    public void draw() {
        if (width_yuv > 0 && height_yuv > 0) {
            GLES30.glUseProgram(mProgram);

            GLES30.glEnableVertexAttribArray(avPosition);
            GLES30.glEnableVertexAttribArray(afPosition);
            GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboIds[0]);
            GLES30.glVertexAttribPointer(avPosition, COORDS_PER_TEX, GLES30.GL_FLOAT, false, COORDS_PER_TEX * 4, 0);
            GLES30.glVertexAttribPointer(afPosition, COORDS_PER_TEX, GLES30.GL_FLOAT, false, COORDS_PER_TEX * 4, vertexData.length * 4);

            GLES30.glActiveTexture(GLES30.GL_TEXTURE0);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, textureId_yuv[0]);
            GLES30.glTexImage2D(GLES30.GL_TEXTURE_2D, 0, GLES30.GL_LUMINANCE, width_yuv, height_yuv, 0,
                    GLES30.GL_LUMINANCE, GLES30.GL_UNSIGNED_BYTE, y);

            GLES30.glActiveTexture(GLES30.GL_TEXTURE1);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, textureId_yuv[1]);
            GLES30.glTexImage2D(GLES30.GL_TEXTURE_2D, 0, GLES30.GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0,
                    GLES30.GL_LUMINANCE, GLES30.GL_UNSIGNED_BYTE, u);

            GLES30.glActiveTexture(GLES30.GL_TEXTURE2);
            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, textureId_yuv[2]);
            GLES30.glTexImage2D(GLES30.GL_TEXTURE_2D, 0, GLES30.GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0,
                    GLES30.GL_LUMINANCE, GLES30.GL_UNSIGNED_BYTE, v);

            GLES30.glUniform1i(sampler_y, 0);
            GLES30.glUniform1i(sampler_u, 1);
            GLES30.glUniform1i(sampler_v, 2);

            GLES30.glDrawArrays(GLES30.GL_TRIANGLE_STRIP, 0, VERTEX_COUNT);

            GLES30.glDisableVertexAttribArray(avPosition);
            GLES30.glDisableVertexAttribArray(afPosition);
        }
    }

    public void release() {
        if (y != null) {
            y.clear();
            y = null;
        }
        if (u != null) {
            u.clear();
            u = null;
        }
        if (v != null) {
            v.clear();
            v = null;
        }
        GLES30.glDeleteProgram(mProgram);
        GLES30.glDeleteBuffers(vboIds.length, vboIds, 0);
        GLES30.glDeleteTextures(textureId_yuv.length, textureId_yuv, 0);
    }

    public int[] getTextureId() {
        return textureId_yuv;
    }

    /**
     * 将YUV420分离出y u v三个分量
     */
    private void extractYUV(byte[] yuvData, byte[] yData, byte[] uData, byte[] vData) {
        if (width_yuv * height_yuv >= 0)
            System.arraycopy(yuvData, 0, yData, 0, width_yuv * height_yuv);
        if (width_yuv * height_yuv / 4 >= 0)
            System.arraycopy(yuvData, width_yuv * height_yuv, uData, 0, width_yuv * height_yuv / 4);
        if (width_yuv * height_yuv / 4 >= 0)
            System.arraycopy(yuvData, width_yuv * height_yuv * 5 / 4, vData, 0, width_yuv * height_yuv / 4);
    }

    private int checkGLError(String op) {
        int error;
        if ((error = GLES30.glGetError()) != GLES30.GL_NO_ERROR) {
            Log.e(TAG, op + "glError, error Code:" + error);
            return error;
        }
        return error;
    }

}
