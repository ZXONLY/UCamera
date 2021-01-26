package com.zxx.camera.mediacodec;

public interface MediaCodecEncoderCallback {
    void onFillInputSurface(MediaFrame frame);

    void onEncodedFrameAvailable(MediaFrame encodeFrame);
}
