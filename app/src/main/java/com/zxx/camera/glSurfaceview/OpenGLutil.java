package com.zxx.camera.glSurfaceview;

import android.content.res.Resources;
import android.opengl.GLES11Ext;
import android.opengl.GLES30;
import android.util.Log;

import javax.microedition.khronos.opengles.GL10;

public class OpenGLutil {
    private static final String TAG = "OpenGLUtils";

    /*
    将Camera的预览数据通过SurfaceTexture传入GPU
     */
    public static int getExternalOESTextureID()
    {
        int[] texture = new int[1];
        GLES30.glGenBuffers(1,texture,0);
        GLES30.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,texture[0]);
        GLES30.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MIN_FILTER,GL10.GL_LINEAR);
        GLES30.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MAG_FILTER,GL10.GL_LINEAR);
        GLES30.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_S,GL10.GL_CLAMP_TO_EDGE);
        GLES30.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_T,GL10.GL_CLAMP_TO_EDGE);
        return texture[0];
    }


    public static int loadShader(int type,String source)
    {
        int shader = GLES30.glCreateShader(type);
        if(shader == GLES30.GL_NONE){
            Log.e(TAG,"creat shader failed! type :"+type);
        }
        GLES30.glShaderSource(shader,source);
        GLES30.glCompileShader(shader);
        int[] compiled = new int[1];
        GLES30.glGetShaderiv(shader,GLES30.GL_COMPILE_STATUS,compiled,0);
        if(compiled[0] == GLES30.GL_FALSE)
        {
            Log.e(TAG,"Error compiling shader. type: "+type+": ");
            Log.e(TAG,GLES30.glGetShaderInfoLog(shader));
            GLES30.glDeleteShader(shader);
            shader = GLES30.GL_NONE;
        }
        return shader;
    }

    public static int createProgram(String vertexSource, String fragmentSource)
    {
        int vertexShader = loadShader(GLES30.GL_VERTEX_SHADER,vertexSource);
        if(vertexShader==GLES30.GL_NONE){
            Log.e(TAG,"load vertex shader failed!");
            return GLES30.GL_NONE;
        }
        int fragmentShader = loadShader(GLES30.GL_FRAGMENT_SHADER,fragmentSource);
        if(fragmentShader==GLES30.GL_NONE){
            Log.e(TAG,"load fragment shader failed!");
            return GLES30.GL_NONE;
        }

        int program = GLES30.glCreateProgram();
        if(program == GLES30.GL_NONE){
            Log.e(TAG,"create program failed!");
            return GLES30.GL_NONE;
        }

        GLES30.glAttachShader(program,vertexShader);
        GLES30.glAttachShader(program,fragmentShader);

        GLES30.glDeleteShader(vertexShader);
        GLES30.glDeleteShader(fragmentShader);

        GLES30.glLinkProgram(program);

        int[] linkstatus = new int[1];
        GLES30.glGetProgramiv(program,GLES30.GL_LINK_STATUS,linkstatus,0);
        if(linkstatus[0]==GLES30.GL_NONE)
        {
            Log.e(TAG,"link program failed!"+GLES30.glGetProgramInfoLog(program));
            GLES30.glDeleteProgram(program);
            return GLES30.GL_NONE;
        }
        return program;
    }
    public static String loadFromAssets(String filename, Resources resources)
    {
        return null;
    }


}
