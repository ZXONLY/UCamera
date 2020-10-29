package com.zxx.camera;

import android.opengl.EGLSurface;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.zxx.camera.Utils.EGLbase;
import com.zxx.camera.renderer.NativeTrangleRender;
import com.zxx.camera.renderer.TriangleRenderer;

public class TriangleActivity extends AppCompatActivity {

    private EGLSurface eglSurface;
    //private EGLbase mEGLbase;
    private SurfaceView surfaceView;
    //private TriangleRenderer mtriangleRenderer;
    private NativeTrangleRender   nativeTrangleRender;
    private Thread mthread;
    private boolean cancle = true;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_triangle);
        surfaceView = findViewById(R.id.egl_surface);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                nativeTrangleRender = new NativeTrangleRender();
                nativeTrangleRender.onSurfaceCreated(holder.getSurface());
            }

            @Override
            public void surfaceChanged(@NonNull final SurfaceHolder holder, int format, final int width, final int height) {

                nativeTrangleRender.onSurfaceChanged(width,height);
//                mthread = new Thread(){
////                    @Override
////                    public void run(){
////                        super.run();
//////                        mEGLbase = new EGLbase();
//////                        mEGLbase.render(holder.getSurface(),width,height);
//////                        mtriangleRenderer = new TriangleRenderer();
////                        nativeTrangleRender.onSurfaceCreated(holder.getSurface());
////                        while(!cancle){
//////                            GLES30.glViewport(0, 0, width, height);
//////
//////                            GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT);
//////                            GLES30.glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
//////                            mtriangleRenderer.drawTriangle();
//////                            mEGLbase.swapBuffers();
////
////                            nativeTrangleRender.onSurfaceChanged(width,height);
////                            try {
////                                Thread.sleep(16);
////                            } catch (InterruptedException e) {
////                                e.printStackTrace();
////                            }
////                        }
////                    }
////                };
////                cancle = false;
////                mthread.start();
            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
//                try {
//                    cancle = true;
//                    //mthread.join();
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
                //mEGLbase.releaseEGL();
            }
        });
    }

}
