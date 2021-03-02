package com.zxx.unique.camera;

public class UqCameraResult {

    public static final int UQ_OK = 0;
    public static final int UQ_FAIL = -1;

    public static final int UQ_UNKNOWN_ERROR = -1;
    //COMMON ERROR
    public static final int UQ_INVALID_PARAM = -100;
    public static final int UQ_INVALID_CONTEXT = -101;
    public static final int UQ_OUT_OF_MEMORY = -102;
    public static final int UQ_OVERFLOW = -103;
    public static final int UQ_IO_ERROR = -104;
    public static final int UQ_INVALID_STAT = -105;
    public static final int UQ_JNI_ERROR = -106;
    public static final int UQ_TIMEOUT = -107;
    public static final int UQ_INVALID_ENV = -108;
    public static final int UQ_MEM_ERROR = -109;
    public static final int UQ_BUFFER_FULL = -110;
    public static final int UQ_BUFFER_EMPTY = -111;
    public static final int UQ_INVALID_HANDLER = -112;

    //MEDIA ERROR
    public static final int UQ_UNSUPPORTED = -200;
    public static final int UQ_VIDEO_ERROR = -201;
    public static final int UQ_AUDIO_ERROR = -202;
    public static final int UQ_INVALID_FRAME_FORMAT = -203;
    public static final int UQ_END_STREAM = -204;
    public static final int UQ_BAD_FILE = -205;
    public static final int UQ_DECODER_ERROR = -206;
    public static final int UQ_ENCODER_ERROR = -207;

    //EGL/GL ERROR
    public static final int UQ_GL_ERROR = -300;
    public static final int UQ_GLINIT_ERROR = -301;
    public static final int UQ_INVALID_SHADER = -302;
    public static final int UQ_INVALID_PROGRAM = -303;
    public static final int UQ_BAD_TEXTURE = -304;
    public static final int UQ_BAD_FRAME_BUFFER = -305;
    public static final int UQ_BAD_GLBUFFER = -306;

    public static final int UQ_EGL_ERROR = -350;
    public static final int UQ_EGL_NOT_INITIALIZED = -351;
    public static final int UQ_EGL_BAD_ACCESS = -352;
    public static final int UQ_EGL_BAD_ALLOC = -353;
    public static final int UQ_EGL_BAD_ATTRIBUTE = -354;
    public static final int UQ_EGL_BAD_CONTEXT = -355;
    public static final int UQ_EGL_BAD_CONFIG = -356;
    public static final int UQ_EGL_BAD_CURRENT_SURFACE = -357;
    public static final int UQ_EGL_BAD_SURFACE = -358;
    public static final int UQ_EGL_BAD_DISPLAY = -359;
    public static final int UQ_EGL_BAD_MATCH = -360;
    public static final int UQ_EGL_BAD_PARAMEUQ = -361;
    public static final int UQ_EGL_BAD_NATIVE_PIXMAP = -362;
    public static final int UQ_EGL_BAD_NATIVE_WINDOW = -363;
    public static final int UQ_EGL_CONTEXT_LOST = -364;

    //Camera
    public static final int UQ_CAMERA_OPEN_FAILED = -401;
    public static final int UQ_CAMERA_SET_PARAMS_FAILED = -402;
    public static final int UQ_CAMERA_HARDWARE_LEVEL_NOT_SUPPORT = -403;
    public static final int UQ_CAMERA_FOCUS_FAILED = -411;
    public static final int UQ_CAMERA_FOCUS_NOT_SUPPORT = -412;
    public static final int UQ_CAMERA_EC_FAILED = -413;
    public static final int UQ_CAMERA_EC_NOT_SUPPORT = -414;
    public static final int UQ_CAMERA_EC_OUT_OF_RANGE = -415;
    public static final int UQ_CAMERA_TORCH_NO_SUPPORT = -416;
    public static final int UQ_CAMERA_TORCH_FAILED = -417;
    public static final int UQ_CAMERA_SWITCH_FLASH_FAILED = -418;
    public static final int UQ_CAMERA_SWITCH_FLASH_NO_SUPPORT = -419;
    public static final int UQ_CAMERA_ZOOM_FAILED = -420;
    public static final int UQ_CAMERA_ZOOM_NO_SUPPORT = -421;
    public static final int UQ_CAMERA_TAKE_PICTURE = -422;
    public static final int UQ_CAMERA_DUPLICATE_OPERATION = -423;
    public static final int UQ_CAMERA_WHILE_BALANCE_NO_SUPPORT = -424;
    public static final int UQ_CAMERA_PREVIEW_FAILED = -425;
    public static final int UQ_CAMERA_AE_LOCK_NO_SUPPORT = -426;
    public static final int UQ_CAMERA_AE_LOCK_FAILED = -427;
    public static final int UQ_CAMERA_VENDOR_AUTH_FAILED = -428;
    public static final int UQ_CAMERA_VENDOR_AUTH_TIMEOUT = -429;
    public static final int UQ_CAMERA_ISO_FAILED = -430;
    public static final int UQ_CAMERA_SHUTUQ_TIME_FAILED = -431;
    public static final int UQ_CAMERA_APERTURE_FAILED = -432;
    public static final int UQ_CAMERA_AF_LOCK_NO_SUPPORT = -433;
    public static final int UQ_CAMERA_AF_LOCK_FAILED = -434;
    public static final int UQ_CAMERA_MF_NO_SUPPORT = -435;
    public static final int UQ_CAMERA_MF_FAILED = -436;
    public static final int UQ_CAMERA_PREVIEWNG_FAILED = -437;  //run-time failed
    public static final int UQ_CAMERA_FOCUS_CANCELLED = -438;

    //Else
    public static final int UQ_SENSE_TIME_ERROR = -500;
    public static final int UQ_CAMERA_NO_PERMISSION = -501;

    //-------------------------------------------------Info definition------------------------------
    //Camera onInfo type & ext
    public class Info {
        //type
        /**
         * Camera预览消息
         */
        public static final int TET_CAMERA_PREVIEW = 0;

        /**
         * Camera Feature Ready消息
         */
        public static final int TET_CAMERA_FEATURE_NOTIFY = 1;

        /**
         * Warning
         */
        public static final int TET_CAMERA_WARNING = 2;

        /**
         * Status 回调
         */
        public static final int TET_CAMERA_STATUS = 3;

        /**
         * stop preview success消息
         */
        public static final int TET_CAMERA_STOP_PREVIEW = 4;

        /**
         * preview size消息
         */
        public static final int TET_CAMERA_PREVIEW_SIZE = 50;

        /**
         * preview size消息
         */
        public static final int TET_CAMERA_RECREATE_SURFACETEXTURE = 51;

        /**
         * Camera 慢动作系列消息
         */
        public static final int TET_CAMERA_SLOW_MOTION = 100;

        //ext
        /**
         * empty ext
         */
        public static final int TEE_CAMERA_DEFAULT = 0;

        /**
         * slow motion is disable now.
         */
        public static final int TEE_CAMERA_SLOW_MOTION_DISABLE = 100;
        /**
         * slow motion is ready to record now.
         */
        public static final int TEE_CAMERA_SLOW_MOTION_READY = 101;
        /**
         * video is recorded.
         */
        public static final int TEE_CAMERA_SLOW_MOTION_VIDEO_DONE = 102;
        /**
         * slow motion is completed.
         */
        public static final int TEE_CAMERA_SLOW_MOTION_FINISH = 103;

        /**
         * close privacy
         */
        public static final int TEE_CAMERA_CLOSED_PRIVACY = 104;

        /**
         * open privacy
         */
        public static final int TEE_CAMERA_OPENED_PRIVACY = 105;
    }

}
