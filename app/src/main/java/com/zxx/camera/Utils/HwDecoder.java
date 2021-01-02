package com.zxx.camera.Utils;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Handler;
import android.os.HandlerThread;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;

//TODO: 这里有额外的线程分配
public class HwDecoder {
    private static String TAG = HwDecoder.class.getSimpleName();

    private static String mimeType = "video/avc";
    private MediaFormat mMediaFormat;
    private MediaCodec mMediaCodec;

    private HandlerThread mHandlerThread;
    private Handler mHandler;

    public static final int OK = 1;

    public HwDecoder(){
        mHandlerThread = new HandlerThread(TAG);
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper());
    }

    private int createDecoder(byte[] spsData, int spsSize, byte[] ppsData, int ppsSize,int srcWidth, int srcHeight){
        mMediaFormat = MediaFormat.createVideoFormat(mimeType,srcWidth,srcHeight);
        if(spsData!=null && spsSize>0){
            mMediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(spsData));
        }
        if(ppsData!=null && ppsSize>0){
            mMediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(ppsData));
        }
        try{
        mMediaCodec = MediaCodec.createDecoderByType(mimeType);
        }catch (IOException e){
            e.printStackTrace();
            return -1;
        }
        return OK;
    }

    public int startDecode(byte[] spsData, int spsSize, byte[] ppsData, int ppsSize, int srcWidth, int srcHeight, Surface surface){
        int ret = createDecoder(spsData, spsSize, ppsData, ppsSize, srcWidth, srcHeight);
        if(ret!=OK){
            return ret;
        }
        mMediaCodec.configure(mMediaFormat,surface,null,0);
        mMediaCodec.start();
        return OK;
    }

}
