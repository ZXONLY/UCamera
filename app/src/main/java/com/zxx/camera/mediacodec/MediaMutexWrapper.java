package com.zxx.camera.mediacodec;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;

public class MediaMutexWrapper {
    private static final String TAG = MediaMutexWrapper.class.getSimpleName();

    private MediaMuxer mMuxer;
    private String mOutputFiles;
    private int mMuxIndex;
    private MediaCodecEncoder mEncoder;
    private boolean mFinish = false;
    private MuxerStatus mStatus = MuxerStatus.UNSET;

    public enum MuxerStatus {
        UNSET,
        INITED,
        STARTED,
        STOPED,
        RELEASED
    }

    public MediaMutexWrapper(String OutputFile, MediaCodecEncoder encoder){
        mOutputFiles = OutputFile;
        mEncoder = encoder;
    }

    public int init(MediaFormat format){
        if(mStatus != MuxerStatus.UNSET){
            Log.e(TAG,"mediaMuxer init failed, current status is" + mStatus );
            return MediaCodecResult.CODEC_INVALID_STATUS;
        }
        try {
            mMuxer = new MediaMuxer(mOutputFiles,MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            mMuxIndex = mMuxer.addTrack(format);
            mStatus = MuxerStatus.INITED;
        } catch (IOException e) {
            Log.e(TAG,"mediaMutex init failed");
            e.printStackTrace();
            return MediaCodecResult.CODEC_FAIL;
        }
        return mMuxIndex;
    }

    public void start(){
        if(mStatus != MuxerStatus.INITED){
            Log.e(TAG,"mediaMuxer start failed, current status is" + mStatus );
            return;
        }
        if(mMuxer != null){
            mMuxer.start();
        }
        mFinish = false;
        mStatus = MuxerStatus.STARTED;
    }

    public void stop(){
        if(mStatus != MuxerStatus.STARTED){
            Log.e(TAG,"mediaMuxer stop failed, current status is" + mStatus );
            return;
        }
        mFinish = true;
        if(mMuxer != null){
            mMuxer.stop();
        }
        mStatus = MuxerStatus.STOPED;
    }

    public void release(){
        if(mStatus != MuxerStatus.UNSET || mStatus != MuxerStatus.RELEASED){
            Log.e(TAG,"mediaMuxer release failed, current status is" + mStatus );
            return;
        }
        if(!mFinish && mStatus != MuxerStatus.STOPED){
            stop();
        }
        if(mMuxer != null){
            mMuxer.release();
            mMuxer = null;
        }
        mStatus = MuxerStatus.UNSET;
    }

    public int writeData(ByteBuffer data, MediaCodec.BufferInfo info){
        if(mStatus != MuxerStatus.STARTED){
            Log.e(TAG,"mediaMuxer writeData failed, current status is" + mStatus );
            return MediaCodecResult.CODEC_INVALID_STATUS;
        }
        mMuxer.writeSampleData(mMuxIndex,data,info);

        return MediaCodecResult.CODEC_OK;
    }

}
