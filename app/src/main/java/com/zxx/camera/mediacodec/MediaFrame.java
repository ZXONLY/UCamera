package com.zxx.camera.mediacodec;

import androidx.annotation.NonNull;

public class MediaFrame {

    public int pixelFormat;

    public byte[] data = null;

    public int[] texIds;
    /**
     * In microsecond
     */
    public long pts;
    /**
     * In microsecond
     */
    public long dts;

    public boolean isKeyFrame;
    public boolean isEndFrame;

    public boolean isValid(){
        if(pixelFormat == MediaCodecInfoWrapper.PixelFormat.GL_YUV_SPLIT3){
            return texIds != null && texIds.length == 3;
        }else if(pixelFormat == MediaCodecInfoWrapper.PixelFormat.GL_OES
        || pixelFormat == MediaCodecInfoWrapper.PixelFormat.GL_RGB24
        || pixelFormat == MediaCodecInfoWrapper.PixelFormat.GL_RGBA32){
            return texIds != null && texIds.length > 0;
        } else {
            return data != null;
        }
    }

    @NonNull
    @Override
    public String toString(){
        return "Format: " + pixelFormat + "; PTS: " + pts +"; DTS: " + dts + "; isEOF" + isEndFrame +
            ": isKeyFrame: " + isKeyFrame;
    }
    
}
