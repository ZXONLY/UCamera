package com.zxx.unique.mediacodec;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Build;
import android.util.Log;

import androidx.annotation.NonNull;

import java.util.concurrent.LinkedBlockingQueue;

public class MediaCodecAsyncEncoder extends MediaCodecEncoder{
    private static final String TAG = MediaCodecAsyncEncoder.class.getSimpleName();

    protected EncoderCallback mEncoderCallback;
    private LinkedBlockingQueue<Integer> mInputBufferIndices = new LinkedBlockingQueue<>();

    @Override
    protected int configRunningMode() {
        mEncoderCallback = new MediaCodecAsyncEncoder.EncoderCallback();
        if(Build.VERSION.SDK_INT > Build.VERSION_CODES.M){
            mMediaCodec.setCallback(mEncoderCallback,mEncodeHandler);
        }else if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP){
            mMediaCodec.setCallback(mEncoderCallback);
        }else {
            return MediaCodecResult.CODEC_UNSUPPORTED;
        }
        return MediaCodecResult.CODEC_OK;
    }

    @Override
    protected int doEncodeBuffer(MediaFrame frame) throws Exception {
        if(mInputBufferIndices.isEmpty()) {
            return MediaCodecResult.CODEC_INPUTBUFFER_UNAVAILABLE;
        }
        return feedEncode(frame,mInputBufferIndices.poll());
    }

    @Override
    public int stopEncode(){
        int ret = super.stopEncode();
        if(ret == MediaCodecResult.CODEC_OK){
            mInputBufferIndices.clear();
        }
        return ret;
    }

    protected class EncoderCallback extends MediaCodec.Callback{

        @Override
        public void onInputBufferAvailable(@NonNull MediaCodec codec, int index) {
            Log.d(TAG,"onInputBufferAvailable: index =  " + index);
            if(!mEncoderSeetings.useSurfaceInput()){
                mInputBufferIndices.offer(index);
            }
        }

        @Override
        public void onOutputBufferAvailable(@NonNull MediaCodec codec, int index, @NonNull MediaCodec.BufferInfo info) {
            Log.d(TAG,"onOutputBufferAvailable: index =  " + index);
            receiveEncodedData(codec,index,info);
        }

        @Override
        public void onError(@NonNull MediaCodec codec, @NonNull MediaCodec.CodecException e) {
            Log.e(TAG,"onError:   " + e.getMessage());
        }

        @Override
        public void onOutputFormatChanged(@NonNull MediaCodec codec, @NonNull MediaFormat format) {
            Log.i(TAG,"onOutputFormatChanged: " + format.toString());
            mMediaFormat = format;
        }
    }
}
