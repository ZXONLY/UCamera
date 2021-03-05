package com.zxx.camera.presenter;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.opengl.EGL14;
import android.opengl.GLES30;
import android.util.Log;
import android.view.Surface;

import com.zxx.camera.Utils.EGLbase;
import com.zxx.unique.mediacodec.MediaCodecEncoder;
import com.zxx.unique.mediacodec.MediaCodecEncoderCallback;
import com.zxx.unique.mediacodec.MediaCodecInfoWrapper;
import com.zxx.unique.mediacodec.MediaCodecResult;
import com.zxx.unique.mediacodec.MediaCodecSettings;
import com.zxx.unique.mediacodec.MediaFrame;
import com.zxx.unique.mediacodec.MediaMutexWrapper;
import com.zxx.camera.renderer.CameraRender;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;
import java.util.concurrent.LinkedBlockingQueue;

public class HWEncodeManager {
    private static final String TAG  = HWEncodeManager.class.getSimpleName();

    private LinkedBlockingQueue<MediaFrame> mOutputFrames = new LinkedBlockingQueue<>();
    private MediaCodecEncoder mEncode;
    private MediaCodecSettings mSettings;
    private MediaMutexWrapper mMutex;
    private boolean mStart = false;

    private volatile int encodeRet = 0;


    public int init(String outputfile){
        MediaCodecSettings.Builder builder = new MediaCodecSettings.Builder();
        builder.setMimeType("video/avc")
                .setInputColorFormat(MediaCodecSettings.COLOR_FORMAT_SURFACE)
                .setFrameRate(20)
                .setBitRate(720 * 1280 * 3/2)
                .setWidth(720)
                .setHeight(1280)
                .setBitRateMode(MediaCodecSettings.BITRATE_MODE_VBR)
                .setIFrameInternal(1)
                .setEncodeProfile(MediaCodecSettings.AVC_PROFILE_HIGH);
        mSettings = builder.build();
        mEncode = MediaCodecEncoder.createEncoder(1);
        int ret = mEncode.initEncoder(mSettings);
        if(ret < 0 )
            return ret;
        mMutex = new MediaMutexWrapper(outputfile,mEncode);
        return MediaCodecResult.CODEC_OK;
    }

    public int start() {
        int ret = mEncode.startEncode();
        if (ret == 0) {
            mStart = true;
            new Thread(feedTask).start();
            new Thread(muxerTask).start();
        }
        return ret;
    }

    public Surface getSurface(){
        if(mSettings.useSurfaceInput()){
            return mEncode.getInputSurface();
        }else {
            return null;
        }
    }


    public int getResult() {
        try {
            int muxer_ret = muxerTask.get();
            if (muxer_ret != MediaCodecResult.CODEC_OK && encodeRet != MediaCodecResult.CODEC_ENCODE_EXCEPETION) {
                encodeRet = muxer_ret;
            }
        } catch (InterruptedException e) {
            Log.e(TAG, "thread wait is error: " + e.getMessage());
            return -1;
        } catch (ExecutionException e) {
            Log.e(TAG, "get muxer thread ret is error: " + e.getMessage());
            return -1;
        }
        return encodeRet;
    }


    private Runnable feedTask = new Runnable() {
        int mInputFrameIndex = 0;
        EGLbase sharedContext;
        CameraRender cameraRender;

        @Override
        public void run() {
            int ret = 0;
            try {
                final int width = mSettings.getmWidth();
                final int height = mSettings.getmHeight();
                int frameRate = mSettings.getmFrameRate();
                //FileInputStream inputStream = new FileInputStream(mYUVPath);
                int size = width * height * 3 / 2;
                int readLength = 0;

                if (mSettings.useSurfaceInput()) {
                    sharedContext = new EGLbase();
                    Surface surface = mEncode.getInputSurface();
                    sharedContext = new EGLbase();
                    sharedContext.render(surface);
//                    sharedContext = EGLbase.create(EGL14.EGL_NO_CONTEXT, width, height, TESharedContext.EGL_RECORDABLE_ANDROID, surface);
                    if (!EGL14.eglGetCurrentContext().equals(sharedContext.getEGLContext())) {
                        sharedContext.makeCurrent();
                    }

                    cameraRender = new CameraRender();
//                    ret = yuvTextureDrawer.init();
//                    if (ret != 0) {
//                        Log.e(TAG, "init yuv renderer failed.");
//                        mStart = false;
//                        return;
//                    }
                }

                mEncode.setEncoderCallback(new MediaCodecEncoderCallback() {
                    @Override
                    public void onFillInputSurface(MediaFrame frame) {
                        if (mSettings.useSurfaceInput()) {
                            GLES30.glViewport(0, 0, width, height);
                            cameraRender.draw(false,mSettings.getmWidth(),mSettings.getmHeight(),mSettings.getmWidth(),mSettings.getmHeight(),90);
                            GLES30.glFinish();
                            sharedContext.setPresentationTime(frame.pts * 1000);
                            sharedContext.swapBuffers();
                        }
                    }

                    @Override
                    public void onEncodedFrameAvailable(MediaFrame encodedFrame) {
                        mOutputFrames.offer(encodedFrame);
                    }
                });

                while (mStart) {
                    MediaFrame frame = new MediaFrame();
                    frame.data = new byte[size];

                    frame.pts = mInputFrameIndex * 1000000 / frameRate;
                    frame.isEndFrame = false;
                    if (mSettings.useSurfaceInput()) {
                        frame.pixelFormat = MediaCodecInfoWrapper.PixelFormat.GL_OES;

                        //cameraRender.setYUVData(width, height, frame.data);
                        frame.texIds = new  int[1];
                        frame.texIds[0] = cameraRender.getTexture();
                    } else {
                        frame.pixelFormat = MediaCodecInfoWrapper.PixelFormat.YUV420P;
                    }

                    do {
                        ret = mEncode.encode(frame);
                        if (ret < 0 && ret != MediaCodecResult.CODEC_INPUTBUFFER_UNAVAILABLE) {
                            if (ret == MediaCodecResult.CODEC_INVALID_STATUS) {
                                Log.e(TAG, "encode the status is error,ret: " + ret);
                                break;
                            }
                            Log.e(TAG, "encode failed!!! ret: " + ret);
                        }
                    } while (ret < 0 && mStart);

                    mInputFrameIndex++;

                }
                Log.d(TAG, "inputFrames: " + mInputFrameIndex);

                /** End of stream */
                MediaFrame frame = new MediaFrame();
                if (mSettings.useSurfaceInput()) {
                    //yuvTextureDrawer.release();
                    //sharedContext.release();
                    frame.pixelFormat = MediaCodecInfoWrapper.PixelFormat.GL_YUV_SPLIT3;
                }else {
                    frame.pixelFormat = MediaCodecInfoWrapper.PixelFormat.YUV420P;
                }
                /** If using an empty buffer, the timestamp will be ignored. */
                frame.pts = mInputFrameIndex * 1000000 / frameRate;
                frame.isEndFrame = true;
                do {
                    ret = mEncode.encode(frame);
                    if (ret < 0 && ret != MediaCodecResult.CODEC_INPUTBUFFER_UNAVAILABLE) {
                        Log.e(TAG, "encode failed!!! ret: " + ret);
                    }
                } while (ret < 0 && mStart);

                //inputStream.close();
            } catch (IOException e) {

            } catch (Exception e) {

                encodeRet = MediaCodecResult.CODEC_ENCODE_EXCEPETION;
                mStart = false;
            }
        }
    };


    private FutureTask<Integer> muxerTask = new FutureTask<Integer>(new Callable<Integer>() {
        final int TRY_GET_EOS = 30;

        @Override
        public Integer call() throws Exception {
            int width = mSettings.getmWidth();
            int height = mSettings.getmHeight();
            MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
            ByteBuffer byteBuffer = ByteBuffer.allocate(width * height * 3 / 2);
            int track = -1;
            int tryGetEOS = 0;
            int ret = MediaCodecResult.CODEC_OK;
            do {
                MediaFormat format = mEncode.getMediaFormat();
                if (format != null) {
                    track = mMutex.init(format);
                }
            } while (track == -1 && mStart);
            mMutex.start();

            try {
                while (mStart) {
                    MediaFrame frame = mOutputFrames.peek();
                    if (frame == null) {
                        if (mEncode.mInputEOS) {
                            if (tryGetEOS == TRY_GET_EOS) {
                                if (mEncode.mOutputFrames >= mEncode.mInputFrames) {
                                    ret = MediaCodecResult.ENCODE_NOT_END;
                                    Log.e(TAG, "encode not to end");
                                    break;
                                } else {
                                    ret = MediaCodecResult.ENCODE_ENDLESS_LOOPER;
                                    Log.e(TAG, "encode endless looper");
                                    break;
                                }
                            } else {
                                tryGetEOS++;
                                Log.w(TAG, "hwecoder try get eos: " + tryGetEOS);
                            }
                        }
                        Thread.sleep(50);
                        continue;
                    }
                    tryGetEOS = 0;
                    info.presentationTimeUs = frame.pts;
                    info.offset = 0;
                    if (frame.isKeyFrame) {
                        info.flags |= MediaCodec.BUFFER_FLAG_KEY_FRAME;
                    }

                    if (frame.isEndFrame) {
                        info.flags |= MediaCodec.BUFFER_FLAG_END_OF_STREAM;
                        info.size = 0;
                        byteBuffer.clear();
                    } else {
                        byteBuffer.clear();
                        byteBuffer.put(frame.data);
                        byteBuffer.position(0);
                        byteBuffer.limit(frame.data.length);
                        info.size = frame.data.length;
                    }
                    ret = mMutex.writeData(byteBuffer, info);
                    Log.i(TAG, "call: writeData");
                    if (ret < 0) {
                        Thread.sleep(50);
                        continue;
                    }
                    mOutputFrames.poll();
                    if (frame.isEndFrame) {
                        ret = MediaCodecResult.CODEC_OK;
                        break;
                    }
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            } finally {
                if(encodeRet != MediaCodecResult.CODEC_ENCODE_EXCEPETION){
                    mEncode.stopEncode();
                    mEncode.releaseEncode();
                }
                mMutex.stop();
                mMutex.release();
                mStart = false;
            }
            return ret;
        }
    });



}
