//
// Created by bytedance on 02/03/2021.
//

#pragma once

typedef int UResult;

#define COMBINE_I64ERROR(ret)  ((int64_t(__LINE__)<<32)|(-(ret)))
#define SEPARATE_I32ERROR(ret)  (-(int)(ret & 0xFFFFFFFF))
#define SEPARATE_I32LINE(ret)  ((int)(ret >> 32))
#define COMBINE_I64ERROR_WITH_EXT(ret,retExt) ((int64_t(retExt<0?(-(retExt)):retExt)<<32)|(-(ret)))
#define SEPARATE_I32EXT(ret)  ((int)(ret >> 32)) //only return positive value of the extra error code

#define UR_RETRY              4
#define UR_ALREADY            3
#define UR_FALSE  2
#define UR_ATTACH_TO_VM 1
#define UR_OK 0
#define UR_FAIL -1
#define UR_IS_RUNNING -2
#define UR_ENGINE_NOT_CREATED -20000
#define UR_EXIT    -10000

// in fact, this is NOT a error, just need more data
#define UR_MORE_DATA   -10

#define UR_UNKNOWN_ERROR -1
//COMMON ERROR
#define UR_INVALID_PARAM -100
#define UR_INVALID_CONTEXT -101
#define UR_OUT_OF_MEMORY -102
#define UR_OVERFLOW -103
#define UR_IO_ERROR -104
#define UR_INVALID_STATE -105
#define UR_JNI_ERROR -106
#define UR_TIMEOUT -107
#define UR_INVALID_ENV -108
#define UR_MEM_ERROR -109
#define UR_BUFFER_FULL -110
#define UR_BUFFER_EMPTY -111
#define UR_INVALID_HANDLER -112
#define UR_NO_SPACE -113   //no space for write file
#define UR_NO_SUCH_FILE -114 //file has been deleted while encoding
#define UR_NO_PIPELINE     -115   // no pipeline now, try again later
#define UR_NOT_FOUND       -116   // not found

//MEDIA ERROR
#define UR_BAD_AUDIO_INFO       -198
#define UR_BAD_WIDTH_OR_HEIGHT  -199
#define UR_UNSUPPORTED -200
#define UR_VIDEO_ERROR -201
#define UR_AUDIO_ERROR -202
#define UR_INVALID_FRAME_FORMAT -203
#define UR_END_STREAM -204
#define UR_BAD_FILE -205
#define UR_DECODER_ERROR -206
#define UR_ENCODER_ERROR -207
#define UR_BAD_TIME_STAMP -208
#define UR_BAD_VIDEO_STREAM -209
#define UR_BAD_AUDIO_STREAM -210

#define UR_MEDIA_CODEC_ENC_EXCEPTION -211 //mediacodec encode exception
#define UR_MEDIA_CODEC_DEC_EXCEPTION -212 //mediacodec decode exception
#define UR_MEDIA_CODEC_SIG_END_EXCEPTION -213 //mediacodec encode signal end exception
#define UR_MEDIA_CODEC_DEQUEUE_OUTPUT_BUFFER_EXCEPTION -214
#define UR_POST_VALIDATE_ERROR -215
#define UR_MEDIA_CODEC_INIT_ERROR -220
#define UR_SOFT_CODEC_PROBE_ERROR -221


#define UR_NO_VIDEO_DECODER -216
#define UR_NO_AUDIO_DECODER -217
#define UR_SWS_CXT_INIT_ERROR -218

#define UR_AUDIO_HW_INIT_FAILURE -230
#define UR_AUDIO_SW_INIT_FAILURE -231
#define UR_AUDIO_SWITCH_TO_SW_FAILURE -232
#define UR_UPLOAD_AS_YOU_COMPILE      -233

#define UR_MEDIA_CODEC_AUDIO_DEQUEUE_INPUT_BUFFER_EXCEPTION  -50010
#define UR_MEDIA_CODEC_AUDIO_DEQUEUE_OUTPUT_BUFFER_EXCEPTION -60010

//EGL/GL ERROR
#define UR_GL_ERROR -300
#define UR_GLINIT_ERROR -301
#define UR_INVALID_SHADER -302
#define UR_INVALID_PROGRAM -303
#define UR_BAD_TEXTURE -304
#define UR_BAD_FRAME_BUFFER -305
#define UR_BAD_GLBUFFER -306
#define UR_BAD_GLID         -307

#define UR_EGL_ERROR -350
#define UR_EGL_NOT_INITIALIZED -351
#define UR_EGL_BAD_ACCESS -352
#define UR_EGL_BAD_ALLOC -353
#define UR_EGL_BAD_ATTRIBUTE -354
#define UR_EGL_BAD_CONTEXT -355
#define UR_EGL_BAD_CONFIG -356
#define UR_EGL_BAD_CURRENT_SURFACE -357
#define UR_EGL_BAD_SURFACE -358
#define UR_EGL_BAD_DISPLAY -359
#define UR_EGL_BAD_MATCH -360
#define UR_EGL_BAD_PARAMETER -361
#define UR_EGL_BAD_NATIVE_PIXMAP -362
#define UR_EGL_BAD_NATIVE_WINDOW -363
#define UR_EGL_CONTEXT_LOST -364


#define UR_EFFECT_GIF_NOT_SUPPROT -1301
#define UR_EFFECT_GIF_FILE_ERROR -1302

#define UR_BEF_EFFECT_ERROR  -501
#define UR_INVALID_EFFECT_HANDLER -502
#define UR_BINGO_EFFECT_ERROR -503
#define UR_EFFECT_EMPTY_DRAW      -504

//
#define UR_RECORD_AV_WRITER_ALREADY_DONE       -504
#define UR_RECORD_AV_WRITER_WAITING_NEXT_FLUSH -505

//--------------ios input error code------------//
#define UR_READER_IOS_ERROR_CODE_BASE          -20000
#define UR_DECODER_FFMPEG_ERROR_CODE_BASE      -21000
#define UR_DECODER_IOS_API_ERROR_BASE          -600000

#define UR_DECODER_NO_FORMAT_CONTEXT           -21101
#define UR_DECODER_NOT_ON_RUNNING              -21102
#define UR_DECODER_ON_FRAME_IN_CACHE           -21103
#define UR_DECODER_ALREADY_OPEN                -21104
#define UR_DECODER_NO_VIDEO_STREAM             -21105

#define UR_READER_CREATE_COLOR_READER_FAILED   -22101
#define UR_READER_CREATE_IMAGE_READER_FAILED   -22102
#define UR_READER_FILE_INFO_IS_NULL            -22103
#define UR_READER_TIMELINE_IS_NULL             -22104
#define UR_READER_GET_FILE_INFO_FAILED         -22105
#define UR_READER_STREAM_COUNT_ERROR           -22106
#define UR_READER_GET_STREAM_INFO_FAILED       -22107