package com.zxx.camera.presenter;

import android.media.MediaActionSound;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Surface;

import java.io.File;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

public class CircularEncoder {

    private static final String TAG = CircularEncoder.class.getSimpleName();
    private static final boolean VERBOSE = false;

    private static final String MIME_TYPE = "video/avc";
    private static final int IFRAME_INTERVAL = 1;

    private EncoderThread mEncoderThread;
    private Surface mInputSurface;
    private MediaCodec mEncoder;

    private interface Callback {

        void fileSaveComplete(int status);

        void bufferStatus(long totalTimeMsec);
    }

    /**
     * @param width
     * @param height
     * @param bitRate
     * @param frameRate
     * @param desiredSpanSec 我们随时希望在缓冲区中保留多少秒的视频
     * @param cb
     * @throws IOException
     */
    public CircularEncoder(int width, int height, int bitRate, int frameRate, int desiredSpanSec,
                           Callback cb) throws IOException {
        if (desiredSpanSec < IFRAME_INTERVAL * 2) {
            throw new RuntimeException("Requested time span is too short: " + desiredSpanSec + " vs. " + (IFRAME_INTERVAL * 2));
        }
        CircularEncoderBuffer encBUffer = new CircularEncoderBuffer(bitRate, frameRate, desiredSpanSec);

        MediaFormat format = MediaFormat.createVideoFormat(MIME_TYPE, width, height);

        format.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        format.setInteger(MediaFormat.KEY_BIT_RATE, bitRate);
        format.setInteger(MediaFormat.KEY_FRAME_RATE, frameRate);
        format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, IFRAME_INTERVAL);

        mEncoder = MediaCodec.createEncoderByType(MIME_TYPE);
        mEncoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        mInputSurface = mEncoder.createInputSurface();
        mEncoder.start();

        mEncoderThread = new EncoderThread(mEncoder,encBUffer,cb);
        mEncoderThread.start();
        mEncoderThread.waitUntilReady();
    }

    public Surface getInputSurface(){
        return mInputSurface;
    }

    public void shutDown(){
        Handler handler = mEncoderThread.getHandle();
        handler.sendMessage(handler.obtainMessage(EncoderThread.EncoderHandler.MSG_SHUTDOWN));
        try {
            mEncoderThread.join();
        } catch (InterruptedException w){
            Log.w(TAG,"Encoder Thread join interrupt");
        }

        if(mEncoder != null){
            mEncoder.stop();
            mEncoder.release();
            mEncoder = null;
        }
    }

    public void frameAvailableSoon(){
        Handler handler = mEncoderThread.getHandle();
        handler.sendMessage(handler.obtainMessage(EncoderThread.EncoderHandler.MSG_FRAME_AVAILABLE_SOON));
    }

    public void saveVideo(File outputFile){
        Handler handler = mEncoderThread.getHandle();
        handler.sendMessage(handler.obtainMessage(EncoderThread.EncoderHandler.MSG_SAVE_VIDEO,outputFile));
    }


    private static class EncoderThread extends Thread {
        private MediaCodec mEncoder;
        private MediaFormat mEncoderFormat;
        private MediaCodec.BufferInfo mBufferInfo;

        private EncoderHandler mHandle;
        private CircularEncoderBuffer mEncBuffer;
        private CircularEncoder.Callback mCallback;
        private int mFrameNum;

        private final Object mLock = new Object();
        private volatile boolean mReady = false;

        public EncoderThread(MediaCodec mediaCodec, CircularEncoderBuffer encBuffer, CircularEncoder.Callback callback) {
            mEncBuffer = encBuffer;
            mEncoder = mediaCodec;
            mCallback = callback;

            mBufferInfo = new MediaCodec.BufferInfo();
        }

        @Override
        public void run() {
            Looper.prepare();
            mHandle = new EncoderHandler(this);
            Log.d(TAG, "encode thread ready");
            synchronized (mLock) {
                mReady = true;
                mLock.notify();
            }

            Looper.loop();

            synchronized (mLock) {
                mReady = false;
                mHandle = null;
            }
            Log.d(TAG, "looper.quit");
        }

        public void waitUntilReady() {
            synchronized (mLock) {
                while (!mReady) {
                    try {
                        mLock.wait();
                    } catch (InterruptedException ie) {
                    }
                }
            }
        }

        public EncoderHandler getHandle() {
            synchronized (mLock) {
                if (!mReady) {
                    throw new RuntimeException("not ready");
                }
            }
            return mHandle;
        }

        public void drainEncode() {
            final int TIMEOUT_USEC = 0;

            //ByteBuffer[] encoderOutputBuffers = mEncoder.getOutputBuffer();
            while (true) {
                int encoderStatus = mEncoder.dequeueOutputBuffer(mBufferInfo, TIMEOUT_USEC);
                if (encoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                    break;
                } else if (encoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    mEncoderFormat = mEncoder.getOutputFormat();
                } else if (encoderStatus < 0) {
                    Log.w(TAG, "unexpected result from encoder,dequeueOutputBuffer: " + encoderStatus);
                } else {
                    ByteBuffer encodeData = mEncoder.getOutputBuffer(encoderStatus);
                    if (encodeData == null) {
                        throw new RuntimeException("encoderOutputBuffer " + encoderStatus + " was null");
                    }

                    if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                        mBufferInfo.size = 0;
                    }

                    if (mBufferInfo.size != 0){
                        encodeData.position(mBufferInfo.offset);
                        encodeData.limit(mBufferInfo.offset + mBufferInfo.size);

                        mEncBuffer.add(encodeData,mBufferInfo.flags,mBufferInfo.presentationTimeUs);
                    }

                    mEncoder.releaseOutputBuffer(encoderStatus,false);

                    if((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0){
                        Log.w(TAG,"rreach end of thr stream");
                        break; //out of while
                    }
                }
            }
        }

        void frameAvailableSoon(){
            if(VERBOSE) Log.d(TAG,"frameAvailableSoon");
            drainEncode();

            mFrameNum++;
            if((mFrameNum%10) == 0){
                mCallback.bufferStatus(mEncBuffer.computeTimeSpanUsec());
            }
        }

        void saveVideo(File outPutFile){
            if(VERBOSE) Log.d(TAG,"savaVideo" + outPutFile);

            int index = mEncBuffer.getFirstIndex();
            if(index<0){
                Log.w(TAG,"Unable to get first index");
                mCallback.fileSaveComplete(1);
                return;
            }
            MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
            MediaMuxer muxer = null;
            int result = -1;
            try {
                muxer = new MediaMuxer(outPutFile.getPath(),MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
                int videoTrack = muxer.addTrack(mEncoderFormat);
                muxer.start();

                do {
                    ByteBuffer buffer = mEncBuffer.getChunk(index,info);
                    muxer.writeSampleData(videoTrack,buffer,info);
                    index = mEncBuffer.getNextIndex(index);
                } while (index >= 0);
                result = 0;
            } catch (IOException e) {
                Log.w(TAG,"mutex failed");
                result = 2;
            } finally {
                if(muxer != null){
                    muxer.stop();
                    muxer.release();
                }
            }

            mCallback.fileSaveComplete(result);
        }

        void shutDown(){
            Looper.myLooper().quit();
        }

        private static class EncoderHandler extends Handler {
            public static final int MSG_FRAME_AVAILABLE_SOON = 1;
            public static final int MSG_SAVE_VIDEO = 2;
            public static final int MSG_SHUTDOWN = 3;

            private WeakReference<EncoderThread> mWeakEncoderThread;

            public EncoderHandler(EncoderThread et) {
                mWeakEncoderThread = new WeakReference<EncoderThread>(et);
            }

            @Override
            public void handleMessage(Message msg) {
                int what = msg.what;

                EncoderThread encoderThread = mWeakEncoderThread.get();
                if (encoderThread == null) {
                    Log.w(TAG, "EncoderHandler: weak ref is null");
                    return;
                }

                switch (what) {
                    case MSG_FRAME_AVAILABLE_SOON:
                        encoderThread.frameAvailableSoon();
                        break;
                    case MSG_SAVE_VIDEO:
                        encoderThread.saveVideo((File)msg.obj);
                        break;
                    case MSG_SHUTDOWN:
                        encoderThread.shutDown();
                        break;
                    default:
                        throw new RuntimeException("unkown message " + what);
                }
            }
        }
    }
}
