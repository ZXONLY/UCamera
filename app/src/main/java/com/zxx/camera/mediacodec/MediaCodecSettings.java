package com.zxx.camera.mediacodec;

import android.media.MediaCodecInfo;
import android.media.MediaFormat;

public class MediaCodecSettings {

    public static final String MIME_TYPE_AVC = MediaFormat.MIMETYPE_VIDEO_AVC;
    public static final String MIME_TYPE_VC1 = MediaFormat.MIMETYPE_VIDEO_HEVC;

    public static final int BITRATE_MODE_CQ = MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ;
    public static final int BITRATE_MODE_VBR = MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR;
    public static final int BITRATE_MODE_CBR = MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CBR;

    public static final int AVC_PROFILE_BASELINE = MediaCodecInfo.CodecProfileLevel.AVCProfileBaseline;
    public static final int AVC_PROFILE_MAIN = MediaCodecInfo.CodecProfileLevel.AVCProfileMain;
    public static final int AVC_PROFILE_HIGH = MediaCodecInfo.CodecProfileLevel.AVCProfileHigh;
    public static final int HEVC_PROFILE_MAIN = MediaCodecInfo.CodecProfileLevel.HEVCProfileMain;

    public static final int COLOR_FORMAT_SURFACE = MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface;

    public static final int COLOR_FORMAT_YUV420 = MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible;


    public String getmMineType() {
        return mMineType;
    }

    public void setmMineType(String mMineType) {
        this.mMineType = mMineType;
    }

    public int getmInputColorFormat() {
        return mInputColorFormat;
    }

    public void setmInputColorFormat(int mInputColorFormat) {
        this.mInputColorFormat = mInputColorFormat;
    }

    public int getmFrameRate() {
        return mFrameRate;
    }

    public void setmFrameRate(int mFrameRate) {
        this.mFrameRate = mFrameRate;
    }

    public int getmIFrameInternal() {
        return mIFrameInternal;
    }

    public void setmIFrameInternal(int mIFrameInternal) {
        this.mIFrameInternal = mIFrameInternal;
    }

    public int getmBitRate() {
        return mBitRate;
    }

    public void setmBitRate(int mBitRate) {
        this.mBitRate = mBitRate;
    }

    public int getmBitRateMode() {
        return mBitRateMode;
    }

    public void setmBitRateMode(int mBitRateMode) {
        this.mBitRateMode = mBitRateMode;
    }

    public int getmEncodeProfile() {
        return mEncodeProfile;
    }

    public void setmEncodeProfile(int mEncodeProfile) {
        this.mEncodeProfile = mEncodeProfile;
    }

    public int getmWidth() {
        return mWidth;
    }

    public void setmWidth(int mWidth) {
        this.mWidth = mWidth;
    }

    public int getmHeight() {
        return mHeight;
    }

    public void setmHeight(int mHeight) {
        this.mHeight = mHeight;
    }

    public boolean useSurfaceInput(){
        return mInputColorFormat == COLOR_FORMAT_SURFACE;
    }

    private String mMineType;

    private int mInputColorFormat;

    private int mFrameRate;

    private int mIFrameInternal;

    private int mBitRate;

    private int mBitRateMode;

    private int mEncodeProfile;

    private int mWidth;

    private int mHeight;

}
