package com.zxx.camera.mediacodec;

public class MediaCodecResult {

    public static int CODEC_OK = 0;

    public static int ENCODE_ENDLESS_LOOPER = 300;
    public static int ENCODE_NOT_END        = 301;

    public static int CODEC_FAIL            = -1;
    public static int CODEC_INVALID_PARAM   = -100;
    public static int CODEC_UNSUPPORTED     = -200;

    public static int CODEC_INITENCODER_FAIL    = -600;
    public static int CODEC_CODECINFO_ERROR     = -601;
    public static int CODEC_GETCOLORFORMAT_FAIL = -602;
    public static int CODEC_SDK_UNSUPPORT       = -603;
    public static int CODEC_INVAILD_VIDEO_SIZE  = -604;
    public static int CODEC_INPUTBUFFER_UNAVAILABLE = -605;
    public static int CODEC_NO_CALLBACK         = -606;
    public static int CODEC_INVALID_STATUS      = -607;
    public static int CODEC_ENCODE_EXCEPETION   = -608;
    public static int CODEC_MUXER_EXCEPETION    = -609;
}
