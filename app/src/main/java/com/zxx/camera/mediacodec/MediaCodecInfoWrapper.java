package com.zxx.camera.mediacodec;

public class MediaCodecInfoWrapper {
    public interface PixelFormat{
        static final int YUV420P = 0;
        static final int NV12 = 4;
        static final int NV21 = 5;
        static final int RGB24 = 7;
        static final int RGBA32 = 12;
        static final int GL_RGB24 = 14;
        static final int GL_RGBA32 = 15;
        static final int GL_OES = 16;
        static final int GL_YUV_SPLIT3 = 17;

    }
}
