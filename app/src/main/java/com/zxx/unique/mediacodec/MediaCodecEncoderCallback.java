package com.zxx.unique.mediacodec;

public interface MediaCodecEncoderCallback {
    void onFillInputSurface(MediaFrame frame);

    void onEncodedFrameAvailable(MediaFrame encodeFrame);
}
