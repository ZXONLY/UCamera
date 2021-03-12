//////
////// Created by bytedance on 10/03/2021.
//////
////
////#include "UFFMpegFileWriter.h"
//////
////// Created by gaochao on 2018/1/3.
//////
////
////#include "TEFFmpegVideoFrame.h"
////#include "TEImageUtils.h"
////#include "TEFFmpegAudioSamples.h"
////#include "TEAudioUtils.h"
////#include "TECoreIds.h"
////#include "TEFFmpegUtils.h"
////#include <libavutil/mathematics.h>
////#include <libavformat/avio.h>
////#include "TEFFMpegFileWriter.h"
////#include "AVFileWriterParams.h"
////#include <new>
////#include "TEFFMpegBufferWriter.h"
////#include <thread>
////#include <string>
////#include "TEAVCodecWrapper.h"
////#include "TECommonUtils.h"
////#include "TEFFmpegRegister.h"
////#include "TEFFmpegVideoPacket.h"
////#include "TEFFmpegAudioPackets.h"
////#if defined(__ANDROID__)
////
////#include "TERuntimeConfig.h"
////#include "monitor/TEMonitorErrCode.h"
////#include <TEFileDecoder.h>
////#include <TEErrorUtils.h>
////#include <TEStreamingCompilerParams.h>
////#include <TEEditorInfo.h>
////#include <ffmpeg/include/libavcodec/internal.h>
////#include <ffmpeg/include/libavformat/avio.h>
////#include <sstream>
////#include <TETimeUtils.h>
////#include <ffmpeg/android/TEAACHwEncoder.h>
////#include <monitor/TEPerfStats.h>
////#include "TEMediaCodecEncLib.h"
////#include "ffmpeg/fdk-aac/ies_fdkaac.h"
////#include "ffmpeg/x264/ies_libx264.h"
////#include "ffmpeg/byte264/ies_libbyte264.h"
////#elif defined(__IOS__)
////#include <CoreVideo/CoreVideo.h>
////#include <libavutil/common.h>
////#include "TEPixelBufferUtils.h"
////#include "TECVPixelBufferFrame.h"
////#include "TEPixelBufferUtils.h"
////#include "TEFFmpegVideoPacket.h"
////#include "TEFFmpegAudioPackets.h"
////#include "TETimeUtils.h"
////#include "TEErrorUtils.h"
////#include "TECompileProbe.h"
////#include "TEFileDecoder.h"
////#include "TEMonitorErrCode.h"
////#include "TEStreamingCompilerParams.h"
////#elif defined(__linux__) || defined(_WIN32)
////#include "TERuntimeConfig.h"
////#include "TEErrorUtils.h"
////#include "ffmpeg/fdk-aac/ies_fdkaac.h"
////#include "ffmpeg/x264/ies_libx264.h"
////#include "TEMonitorErrCode.h"
////#include "TETimeUtils.h"
////#include <TEFileDecoder.h>
////#include <TEStreamingCompilerParams.h>
////#include <libavformat/avio.h>
////#include <sstream>
////#endif
////
////#ifdef _WIN32
////#include "libavutil/hwcontext_d3d11va.h"
////#include "mfx/mfxstructures.h"
////#include "mfx/mfxvideo++.h"
////#include "TED3D9VideoFrame.h"
////#include "TEIntelVPP.h"
////#include "TEGpuDeviceManager.h"
////#include "TED3D11VideoFrame.h"
////#endif
//////static const char * const ies_x264_preset_names[] = { "ultrafast", "superfast", "veryfast", "faster", "fast", "medium", "slow", "slower", "veryslow", "placebo", 0 };
////
////static const char *TAG = "TEFFMpegFileWriter";
////
//////bool
//////createFFMpegFileWriter(const string &in_filePath, int in_flags, ITEAVFileWriter **out_ppIWriter) {
//////
//////    TEFFMpegFileWriter *writer = new(nothrow) TEFFMpegFileWriter(in_flags);
//////    if (writer == nullptr)
//////        return false;
//////
//////    vector<string> path;
//////    path.push_back(in_filePath);
//////
//////    if (!writer->openWriter(path)) {
//////        VELOGE("createWriter failed, %s", in_filePath.c_str());
//////        writer->release();
//////        return false;
//////    }
//////
//////    TEFFmpegRegister::enableFFMpegLogCat(true);
//////
//////    *out_ppIWriter = static_cast<ITEAVFileWriter *>(writer);
//////    return true;
//////}
////int64_t seek_user(void *opaque, int64_t offset, int whence) {
////#if defined(__IOS__)
////    TEIOBufferContext *pIoBufCtx = (TEIOBufferContext *)opaque;
////    AVIOContext *pIoCtx = pIoBufCtx->avioCtx;
////#else
////    AVIOContext* pIoCtx = (AVIOContext*)opaque;
////#endif
////    if( pIoCtx != nullptr ) {
////        switch(whence) {
////            case SEEK_CUR:
////                pIoCtx->pos += offset;
////                break;
////            case SEEK_SET:
////                pIoCtx->pos = offset;
////                break;
////        }
////
////        return pIoCtx->pos;
////    }
////
////    return -1;
////}
////
////int write_packet(void *opaque, uint8_t *buf, int buf_size) {
////    int ret = 0;
////    TEIOBufferContext* teioCtx = (TEIOBufferContext*)opaque;
////
////    if (teioCtx == nullptr || teioCtx->avioCtx == nullptr)
////    {
////        return 0;
////    }
////
////    if( teioCtx->file != nullptr ) {
////        fseek(teioCtx->file, teioCtx->avioCtx->pos, SEEK_SET);
////        fwrite(buf, 1, buf_size, teioCtx->file);
////    }
////
////    if (teioCtx->avioCtx->pos < TE_AV_MOV_HEADER_SIZE) {  //cache header
////
////        if (teioCtx->headerCacheBufSize < (teioCtx->avioCtx->pos + buf_size)) {
////            te_io_realloc_buf(&teioCtx->headerCacheBuffer, teioCtx->headerCacheBufSize,
////                              teioCtx->avioCtx->pos + buf_size);
////        }
////        memcpy(teioCtx->headerCacheBuffer + teioCtx->avioCtx->pos, buf, buf_size);
////
////    } else if (teioCtx->readytoWriteTrailer) {            //cache trailer
////
////        if (teioCtx->trailerCacheBufStart == 0) {
////            teioCtx->trailerCacheBufStart = teioCtx->currentWritten;
////        }
////
////        int64_t currentPos = teioCtx->avioCtx->pos - teioCtx->trailerCacheBufStart;
////        int64_t needBufSize = currentPos + buf_size;
////
////        if (teioCtx->trailerCacheBufSize < needBufSize) {
////            int64_t newSize = teioCtx->trailerCacheBufSize * 2 > needBufSize ? teioCtx->trailerCacheBufSize * 2 : needBufSize * 2;
////
////            te_io_realloc_buf(&teioCtx->trailerCacheBufer, teioCtx->trailerCacheBufSize, newSize);
////        }
////
////        memcpy(teioCtx->trailerCacheBufer + currentPos, buf, buf_size);
////
////        if (needBufSize > teioCtx->trailerCacheBufPos)
////            teioCtx->trailerCacheBufPos = needBufSize;
////    } else if (teioCtx->sendBufFunc) {
////        //send buf to up model
////        teioCtx->sendBufFunc(buf, teioCtx->avioCtx->pos, buf_size, false, teioCtx->userData);
////    }
////
////    //update write pos
////    if (teioCtx->avioCtx->pos + buf_size > teioCtx->currentWritten)
////        teioCtx->currentWritten = teioCtx->avioCtx->pos + buf_size;
////
////    return ret;
////}
////
////TEResult ThreadWriteAVPacket::customEvent(TEMsg *pMsg) {
////    VELOGV("recive msg 0x%x", pMsg->what);
////    if (pMsg == nullptr) {
////        VELOGE("Msg is null!");
////        return TER_INVALID_PARAM;
////    }
////    int ret;
////    switch (pMsg->what) {
////        case TE_MSG_FILE_WRITER_PKT: {
////            ret = av_interleaved_write_frame((AVFormatContext *) pMsg->data,
////                                             (AVPacket *) pMsg->opaque);
////            AVPacket *tmp = (AVPacket *) pMsg->opaque;
////            av_packet_free(&tmp);
////            m_nPktCnt++;
////            break;
////        }
////        default: {
////            VELOGE("Error: %d", pMsg->what);
////            break;
////        }
////
////    }
////    return TER_OK;
////}
////
////TEFFMpegFileWriter::TEFFMpegFileWriter(int flags, int64_t audioFlags) :
////        TELightUnknown(), m_bUseMultiThreadEncoding(true), m_bIsValid(false), m_iFlags(flags), m_audioFlags(audioFlags), m_pOutputFormatCtx(nullptr), m_pVideoStream(nullptr), m_pVideoEncCtx(nullptr),
////        m_bIsFlushing(false), m_bIsSendFull(false), m_pInputFormatCtx(nullptr), m_pInputFormatCtxV(nullptr), m_pTEIOBufCtx(nullptr), m_pWriteIOCtx(nullptr), m_bStarted(false),
////        m_bFileHeaderReady(false), m_bIsFirstRemuxVideo(true), m_remuxVideoPtsStart(0), m_remuxVideoDtsStart(0), m_bIsVideoPacketReceived(false), m_remuxLastFrameDts(0), m_bIsEnableMoovFront(false) {
////
////    m_pEncVideoFrame = av_frame_alloc();
////    m_pBgrData = nullptr;
////    if (m_iFlags & TE_AV_FILE_WRITER_FLAG_GIF) {
////        m_pBgrData = (uint8_t *) malloc(4 * 256);
////        memset(m_pBgrData, 0, 4 * 256);
////    }
////
////    if (m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER) {
////        //use malloc here, since it is a ffmpeg c based value
////        m_pEncVideoFrame->opaque = av_mallocz(sizeof(STETextureFrameOpaque));
////    }
////
////    m_bIsCompileSoftInfo = false;
////    if (m_iFlags & TE_AV_FILE_WRITER_FLAG_COMPILE_SOFT_INFO) {
////        m_bIsCompileSoftInfo = true;
////    }
////
////    if (m_iFlags & TE_AV_FILE_WRITER_FLAG_ALPHA_MOV) {
////        m_bAlphaMovEncoding = true;
////    }
////
////    m_pEncAudioFrame = av_frame_alloc();
////
////    m_pEncVideoPkt = av_packet_alloc();
////    m_pEncAudioPkt = av_packet_alloc();
////
////    mFileWriter = NULL;
////
////#if defined(__IOS__)
////    m_pVTConfigData = (TEFFMpegVTConfig) {
////        .hasBFrame = true,
////        .avgBitRate = 2560000,
////        .profile = VE_H264_PROF_HIGH,
////        .profileLevel = 41,
////    };
////
////            m_creationDate = "";
////#endif
////    m_writeMode = VIDEO_WRITING_ENCODE_AND_MUX;
////    m_iCodecID = AV_CODEC_ID_H264;
////    m_bProfileIsBaseline = false;
////    m_iAssumedVPktDuration = av_rescale_q(1, {1,DEFAULT_FPS}, {1,TE_TIME_BASE});
////}
////
////TEFFMpegFileWriter::~TEFFMpegFileWriter() {
////    if (m_pEncVideoFrame->opaque) {
////        av_free(m_pEncVideoFrame->opaque);
////    }
////#if defined(__IOS__)
////    if (m_pEncVideoFrame) {
////        TEReleasePixelBufferInAVFrame(m_pEncVideoFrame);
////    }
////#endif
////    av_frame_free(&m_pEncVideoFrame);
////    av_frame_free(&m_pEncAudioFrame);
////
////    av_packet_free(&m_pEncVideoPkt);
////    av_packet_free(&m_pEncAudioPkt);
////
////    if (m_pBgrData != nullptr) {
////        free(m_pBgrData);
////        m_pBgrData = nullptr;
////    }
////    if (m_pTEIOBufCtx) {
////        if (m_pTEIOBufCtx->headerCacheBuffer) {
////            delete m_pTEIOBufCtx->headerCacheBuffer;
////            m_pTEIOBufCtx->headerCacheBuffer = nullptr;
////        }
////        if (m_pTEIOBufCtx->trailerCacheBufer) {
////            delete m_pTEIOBufCtx->trailerCacheBufer;
////            m_pTEIOBufCtx->trailerCacheBufer = nullptr;
////        }
////        delete m_pTEIOBufCtx;
////        m_pTEIOBufCtx = nullptr;
////    }
////    VELOGI("%s, %d", __FUNCTION__, __LINE__);
////    cleanup();
////
////    if (reportPtr != nullptr) {
////        if (TERuntimeConfig::ve_CompileReport > 1) {
////            reportPtr->printReport();
////        } else if (TERuntimeConfig::ve_CompileReport > 0 && reportPtr->hasError()) {
////            reportPtr->printReport();
////        }
////    }
////}
////
////bool TEFFMpegFileWriter::nonDelegatingQueryInterface(REFIID in_rIID, void **out_ppv) {
////    if (in_rIID == IID_ITEAVFileWriter)
////        return getInterface(static_cast<ITEAVFileWriter *>(this), out_ppv);
////    return TELightUnknown::nonDelegatingQueryInterface(in_rIID, out_ppv);
////}
////
////bool TEFFMpegFileWriter::isSupportGPUInputVideo() {
////#if defined(__ANDROID__)
////    return (avcodec_find_encoder_by_name(ANDROID_HW_ENCODER_NAME) != nullptr);
////#else
////    return false;
////#endif
////}
////
////
////static int pos = 0;
////
////int TEFFMpegFileWriter::writeAVPacketSafe(AVPacket *pkt, int type) {
////
////    int ret = 0;
////    std::lock_guard<std::mutex> guard(m_writeLock);
////
////    if (m_bFileHeaderReady) {   // 只要 m_bFileHeaderReady 为 true，尽可能flush
////        // write cached audio packet
////        if (!m_listAudioPkt.empty()) {
////            VELOGI("TEFFMpegFileWriter write cached audio packet size: %zu", m_listAudioPkt.size());
////            for (auto pPkt : m_listAudioPkt) {
////                int64_t pts = pPkt->pts;
////                int size = pPkt->size;
////                ret = av_interleaved_write_frame(m_pOutputFormatCtx, pPkt);
////                if (ret < 0) {
////                    TELogcat::LogFFMpegError(TAG, ret,
////                                             "TEFFMpegFileWriter writeAVPacketSafe av_interleaved_write_frame() audio "
////                                             "failed 1, pts: %" PRId64 ", data size: %d",
////                            pts, size);
////                }
////                av_packet_free(&pPkt);
////            }
////            m_listAudioPkt.clear();
////        }
////        // write cached video packet
////        if (!m_listVideoPkt.empty()) {
////            VELOGI("write cached video packet size: %zu", m_listVideoPkt.size());
////            for (auto videoPkt : m_listVideoPkt) {
////                ret = av_interleaved_write_frame(m_pOutputFormatCtx, videoPkt);
////                if (ret < 0) {
////                    TELogcat::LogFFMpegError(TAG, ret, "av_interleaved_write_frame() video failed");
////                }
////                av_packet_free(&videoPkt);
////            }
////            m_listVideoPkt.clear();
////        }
////    }
////
////    if (type == AVMEDIA_TYPE_AUDIO) {
////        if (!m_bFileHeaderReady) {
////            if (pkt) {
////                AVPacket *pkt_new = av_packet_alloc();
////                av_packet_move_ref(pkt_new, pkt);
////                if (pkt_new->pts < 0) {
////                    VELOGE("TEFFMpegFileWriter writeAVPacketSafe, push_back , pts: %" PRId64
////                    ", data size: %d, list size: %zu",
////                            pkt_new->pts, pkt_new->size, m_listAudioPkt.size());
////                }
////                m_listAudioPkt.push_back(pkt_new);
////            }
////        } else {
////            ret = av_interleaved_write_frame(m_pOutputFormatCtx, pkt);
////            if (ret < 0) {
////                TELogcat::LogFFMpegError(
////                        TAG, ret, "TEFFMpegFileWriter writeAVPacketSafe av_interleaved_write_frame() audio failed 2");
////            }
////            if (pkt != nullptr)
////                av_packet_unref(pkt);
////        }
////    } else {
////        if (!m_bFileHeaderReady) {
////            if (pkt) {
////                AVPacket *pkt_new_video = av_packet_alloc();
////                av_packet_move_ref(pkt_new_video, pkt);
////                m_listVideoPkt.push_back(pkt_new_video);
////            }
////        } else {
////            ret = av_interleaved_write_frame(m_pOutputFormatCtx, pkt);
////            if (pkt != nullptr)
////                av_packet_unref(pkt);
////        }
////    }
////
////    return ret;
////}
////
////void TEFFMpegFileWriter::cleanup() {
////
////    if( mFileWriter != nullptr ) {
////        fclose(mFileWriter);
////        mFileWriter = nullptr;
////    }
////
////    if (!m_listAudioPkt.empty()) {
////        VELOGE("TEFFMpegFileWriter audio packet size: %zu", m_listAudioPkt.size());
////        for (auto audioPkt : m_listAudioPkt) {
////            if (audioPkt) {
////                av_packet_free(&audioPkt);
////            }
////        }
////        m_listAudioPkt.clear();
////    }
////    if (!m_listVideoPkt.empty()) {
////        VELOGE("TEFFMpegFileWriter video packet size: %zu", m_listVideoPkt.size());
////        for (auto videoPkt : m_listVideoPkt) {
////            if (videoPkt) {
////                av_packet_free(&videoPkt);
////            }
////        }
////        m_listVideoPkt.clear();
////    }
////
////    if (m_pOutputFormatCtx) {
////        // Close video codec context
////
////        if (m_pVideoEncCtx && avcodec_is_open(m_pVideoEncCtx)) {
////            if (m_pVideoEncCtx->opaque) {
////                av_free(m_pVideoEncCtx->opaque);
////                m_pVideoEncCtx->opaque = nullptr;
////            }
////            avcodec_free_context(&m_pVideoEncCtx);
////        }
////
////        // Close audio codec contexts
////        for (auto &ctx : m_pAudioEncCtx) {
////            if (ctx && avcodec_is_open(ctx))
////                avcodec_free_context(&ctx);
////        }
////
////        if (!(m_iFlags & TE_AV_FILE_WRITER_FLAG_BUFFER)) {
////            if( m_pOutputFormatCtx->pb )
////                avio_closep(&m_pOutputFormatCtx->pb);
////        } else {
////            if (m_pOutputFormatCtx->pb) {
////                if (m_pOutputFormatCtx->pb->buffer) {
////                    av_freep(&m_pOutputFormatCtx->pb->buffer);
////                }
////                m_pOutputFormatCtx->pb = nullptr;
////            }
////            if (m_pWriteIOCtx) {
////                av_freep(&m_pWriteIOCtx);
////            }
////        }
////
////        avformat_free_context(m_pOutputFormatCtx);
////        m_pOutputFormatCtx = nullptr;
////        m_pVideoStream = nullptr;
////        m_pAudioStreams.clear();
////        m_pAudioEncCtx.clear();
////    }
////    te_avformat_close_input_custom(&m_pInputFormatCtx);
////    te_avformat_close_input_custom(&m_pInputFormatCtxV);
////    m_pMapUserMetaData.clear();
////    m_sFilePath.clear();
////    m_bStarted = false;
////    VELOGD("call %s %d frameCount=%" PRId64 " writeCountV=%d writeCountA=%d",
////            __FUNCTION__,
////            __LINE__,
////            m_iFrameNum,
////            m_iWriteVideoFrameCount,
////            m_iWriteAudioFrameCount);
////}
////
////static void custom_log_callback(void *ptr, int level, const char *fmt, va_list vl)
////{
////#if !defined(__IOS__)
////    if( TECompileProbe::mParserEncodeInfo != nullptr ) {
////        TECompileProbe::mParserEncodeInfo->setEncodeInfo(fmt, vl); // 传入编码信息
////    }
////#endif
////
////    return;
////}
////
////int64_t TEFFMpegFileWriter::openWriter(const vector<string> filePath) {
////
////    // default output format to be "mp4" if file extension is absent
////    auto oformat = av_guess_format(nullptr, filePath[0].c_str(), nullptr);
////    if (!oformat) {
////        oformat = av_guess_format("mp4", filePath[0].c_str(), nullptr);
////    }
////    if (!oformat) {
////        VELOGE("[%s:%d]can't deduce oformat, filename: %s", __FILE__, __LINE__, filePath[0].c_str());
////        addReportSimpleErrorInfo("[openWriter]invalid path", filePath[0].c_str());
////        return AVERROR_MUXER_NOT_FOUND;
////    }
////
////    // Create output format context
////    const int ret = avformat_alloc_output_context2(&m_pOutputFormatCtx, oformat, nullptr, filePath[0].c_str());
////    if (ret < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "avformat_alloc_output_context2() failed");
////        m_pOutputFormatCtx = nullptr;
////        addReportSimpleErrorInfo("[openWriter]invalid path", filePath[0].c_str());
////        return COMBINE_I64ERROR_WITH_EXT(ret,AVFORMAT_ALLOC_OUTPUT_CONTEXT2_FAILED);
////    }
////
////    if (m_iFlags & TE_AV_FILE_WRITER_FLAG_BUFFER) {
////        //TODO cache buffer size need set or not?
////        mFileWriter = (FILE*)fopen(filePath[0].c_str(), "wb");
////        _createIOContext(512*1024);
////        m_pOutputFormatCtx->pb = m_pWriteIOCtx;
////
////    }
////
////    if(m_iFlags & TE_AV_FILE_WRITER_FLAG_GIF) {
////
////        m_pOutputFormatCtx->oformat->video_codec = AV_CODEC_ID_GIF;
////    } else if (m_iFlags & TE_AV_FILE_WRITER_FLAG_SINGLE_VIDEO) {
////        m_pOutputFormatCtx->oformat->video_codec = AV_CODEC_ID_H264;
////    } else if (m_iFlags & TE_AV_FILE_WRITER_FLAG_MPEG4) {
////        m_pOutputFormatCtx->oformat->video_codec = AV_CODEC_ID_MPEG4;
////    } else {
////        m_pOutputFormatCtx->oformat->video_codec = AV_CODEC_ID_H264;
////    }
////
////    m_sFilePath = filePath[0];
////    m_i64LastWriteVideoPts = AV_NOPTS_VALUE;
////    m_nAudioBuffSize = 0;
////    VELOGD("kelvin test TEFFMpegFileWriter path:%s", m_sFilePath.c_str());
////    return TER_OK;
////}
////
////
////bool TEFFMpegFileWriter::_createIOContext(int cachebufferSize) {
////
////    m_pWriteIOCtx = NULL;
////
////    if (!(m_iFlags & TE_AV_FILE_WRITER_FLAG_BUFFER)) {
////        VELOGE("m_iFlags doesn't include FLAG_BUFFER");
////        return false;
////    }
////
////    uint8_t *avio_ctx_buffer = NULL;
////    size_t avio_ctx_buffer_size = cachebufferSize;
////
////    avio_ctx_buffer = (uint8_t*)av_malloc(avio_ctx_buffer_size);
////
////    if (avio_ctx_buffer == NULL) {
////        VELOGE("av_malloc() failed");
////        return false;
////    }
////
////    m_pTEIOBufCtx = new TEIOBufferContext();
////    m_pWriteIOCtx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size, 1, m_pTEIOBufCtx, NULL, &write_packet, &seek_user);
////
////    m_pWriteIOCtx->seekable = AVIO_SEEKABLE_NORMAL;
////    m_pTEIOBufCtx->avioCtx = m_pWriteIOCtx;
////
////    m_pTEIOBufCtx->file = mFileWriter;
////
////    if (!m_pWriteIOCtx) {
////        VELOGE("avio_alloc_context() failed");
////        av_freep(&avio_ctx_buffer);
////        return false;
////    }
////
////    return true;
////}
////
////void TEFFMpegFileWriter::_flushIOBufCache() {
////    if (!(m_iFlags & TE_AV_FILE_WRITER_FLAG_BUFFER)) {
////        VELOGE("flags not match FLAG_BUFFER");
////        return;
////    }
////
////    if (!m_pTEIOBufCtx || !m_pTEIOBufCtx->sendBufFunc || !m_pTEIOBufCtx->userData) {
////        VELOGE("TE IO not created crotect");
////        return;
////    }
////
////    //flush header
////    if ( m_pTEIOBufCtx && m_pTEIOBufCtx->headerCacheBuffer != nullptr )
////        m_pTEIOBufCtx->sendBufFunc(m_pTEIOBufCtx->headerCacheBuffer, 0, m_pTEIOBufCtx->headerCacheBufSize, false, m_pTEIOBufCtx->userData);
////
////    //flush trailer
////    if ( m_pTEIOBufCtx && m_pTEIOBufCtx->trailerCacheBufer != nullptr )
////#if defined(__IOS__)
////        m_pTEIOBufCtx->sendBufFunc(m_pTEIOBufCtx->trailerCacheBufer, m_pTEIOBufCtx->trailerCacheBufStart, m_pTEIOBufCtx->trailerCacheBufPos, true, m_pTEIOBufCtx->userData);
////#else
////        m_pTEIOBufCtx->sendBufFunc(m_pTEIOBufCtx->trailerCacheBufer, m_pTEIOBufCtx->trailerCacheBufStart, m_pTEIOBufCtx->trailerCacheBufSize, true, m_pTEIOBufCtx->userData);
////#endif
////
////    return;
////}
////
////bool TEFFMpegFileWriter::setMuxerBufferModeParams(TE_IO_SEND_BUF send_buf, void* userData) {
////    if (!(m_iFlags & TE_AV_FILE_WRITER_FLAG_BUFFER)) {
////        VELOGE("flags not match FLAG_BUFFER");
////        return false;
////    }
////
////    if (!m_pTEIOBufCtx) {
////        VELOGE("TE IO buf not created");
////        return false;
////    }
////
////    m_pTEIOBufCtx->sendBufFunc = send_buf;
////    m_pTEIOBufCtx->userData = userData;
////
////    return true;
////}
////
////bool TEFFMpegFileWriter::isValid() const {
////    return m_pOutputFormatCtx != nullptr;
////}
////
////int64_t TEFFMpegFileWriter::start() {
////    if (!isValid()) {
////        addReportSimpleErrorInfo("TEFFMpegFileWriter::start()", "output format context is null");
////        return TER_INVALID_CONTEXT;
////    }
////
////    if (m_bStarted) {
////        addReportSimpleErrorInfo("TEFFMpegFileWriter::start()", "writer has started");
////        return TER_INVALID_STATE;
////    }
////
////    VELOGD("TEFFMpegFileWriter started");
////
////    // Open the output file
////    int ret= 0;
////    if (!(m_iFlags & TE_AV_FILE_WRITER_FLAG_BUFFER)) {
////        ret = avio_open(&m_pOutputFormatCtx->pb, m_sFilePath.c_str(), AVIO_FLAG_WRITE);
////        if (ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ret, "avio_open() failed file_path=%s", m_sFilePath.c_str());
////            cleanup();
////            addReportSimpleErrorInfo("TEFFMpegFileWriter::start()", "invalid output file path");
////            return COMBINE_I64ERROR_WITH_EXT(ret,TER_MEDIA_CODEC_INIT_ERROR);
////        }
////    }
////    if (!m_strGlobalMetaData.empty()) {
////        copyGlobalMetadata(m_pOutputFormatCtx, m_strGlobalMetaData);
////    }
////
////    if (!m_pMapUserMetaData.empty()) {
////        copyUserMetadata(m_pOutputFormatCtx, m_pMapUserMetaData);
////    }
////
////#if defined(__IOS__)
////    av_dict_set(&m_pOutputFormatCtx->metadata, "creation_time", m_creationDate.c_str(), 0);
////#endif
////
////
////    // delay the write header when hw encode
////    if (m_videoEncodeExtradataReady) {
////        // Write the stream header, if any
////        m_writeLock.lock();
////        AVDictionary *opt = nullptr;
////        av_dict_set(&opt, "movflags", "use_metadata_tags", AV_DICT_MULTIKEY);
////#ifdef _WIN32
////        if (m_pVideoEncCtx && (m_pVideoEncCtx->codec->name == "h264_qsv"s || m_pVideoEncCtx->codec->name == "h264_amf"s)) {
////            av_dict_set(&opt, "movflags", "write_colr", AV_DICT_MULTIKEY);
////        }
////#endif
////        if (m_bIsEnableMoovFront) {
////            av_dict_set(&opt, "movflags", "faststart", AV_DICT_MULTIKEY);
////        }
////        ret = avformat_write_header(m_pOutputFormatCtx, &opt);
////        av_dict_free(&opt);
////        m_writeLock.unlock();
////
////        if (ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ret, "avformat_write_header() failed");
////            cleanup();
////            m_bFileHeaderReady = false;
////            addReportSimpleErrorInfo("TEFFMpegFileWriter::start()", "avformat_write_header() failed");
////            return COMBINE_I64ERROR_WITH_EXT(ret, AVFORMAT_WRITE_HEADER_FAILED);
////        } else {
////            m_bFileHeaderReady = true;
////            VELOGI("TEFFMpegFileWriter start write header success");
////        }
////    }
////
////    m_iFrameNum = 0;
////    m_bStarted = true;
////    m_i64LastPts = AV_NOPTS_VALUE;
////    m_i64LastWriteVideoPts = AV_NOPTS_VALUE;
////    m_nAudioBuffSize = 0;
////    m_bFirstFrameEncoded = false;
////    m_i64FisrtFrameEncodePts = 0L;
////    m_i64FisrtFrameEncodeDts = 0L;
////    m_iWriteVideoFrameCount = 0;
////    m_iWriteAudioFrameCount = 0;
////    m_iTotalWriteVideoTime = 0;
////    m_iTotalWriteAudioTime = 0;
////    m_bIsVideoPacketReceived = false;
////    return TER_OK;
////}
////
////bool TEFFMpegFileWriter::copyGlobalMetadata(AVFormatContext *videoEncoderCtx,
////                                            const std::string &srcPath) {
////    TEAVFormatContext *fmt_ctx = new TEAVFormatContext();
////    int ret;
////    int ret_avformat = -1;
////    ret_avformat = te_avformat_open_input_custom(&fmt_ctx, srcPath.c_str(), NULL, NULL);
////    ret = ret_avformat;
////    if (ret < 0) {
////        return false;
////    }
////
////    if (fmt_ctx->formatContext->metadata != nullptr) {
////        av_dict_copy(&videoEncoderCtx->metadata, fmt_ctx->formatContext->metadata, 0);
////    }
////    avformat_close_input(&fmt_ctx->formatContext);
////
////    if (videoEncoderCtx->metadata) {
////        string strEncrypt = "encrypt=false";
////        ret = av_dict_set(&videoEncoderCtx->metadata, "comment", strEncrypt.c_str(), 0);
////        if (ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ret, "av_dict_set() failed !");
////            return false;
////        }
////    }
////    return true;
////}
////
////bool TEFFMpegFileWriter::setUserMetadata(const map<string, string> &metaData) {
////    m_pMapUserMetaData = metaData;
////    return true;
////}
////
////bool TEFFMpegFileWriter::copyUserMetadata(AVFormatContext *videoEncoderCtx, const map<string, string> &metaData) {
////    for (auto &meta : metaData) {
////        int ret = av_dict_set(&videoEncoderCtx->metadata, meta.first.c_str(), meta.second.c_str(), 0);
////        if (ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ret, "av_dict_set() failed. key: %s, value: %s", meta.first.c_str(), meta.second.c_str());
////        }
////    }
////    return true;
////}
////
////int64_t TEFFMpegFileWriter::writeVideoFrame(ITEVideoFrame *in_pIVideoFrame, int64_t in_streamTime) {
////    VELOGD("TEFFMpegFileWriter::writeVideoFrame enter");
////    int64_t startAt = TETimeUtils::getCurrentTime();
////
////    int64_t ret;
////
////    if (in_pIVideoFrame) {
////        ret = _writeVideoFrame(in_pIVideoFrame, in_streamTime);
////    } else {
////        m_iWriteVideoFrameNULLCount++;
////        ret = _flushVideo();
////    }
////    if (reportPtr != nullptr) {
////        char data[100] = { 0 };
////        sprintf(data, "null_frame=%d good_frame=%d ", m_iWriteVideoFrameNULLCount, m_iWriteVideoFrameCount);
////        reportPtr->addCommonInfo("video_frame_count: ", data);
////    }
////
////    m_iTotalWriteVideoTime += TETimeUtils::getCurrentTime() - startAt;
////    VELOGV("TEFFMpegFileWriter::writeVideoFrame index=%d  costtotal= %" PRId64, frameIndex, m_iTotalWriteVideoTime);
////    frameIndex++;
////    return ret;
////}
////
////int64_t TEFFMpegFileWriter::writeVideoPacket(ITEVideoFrame *in_pIVideoPacket, int64_t in_streamTime) {
////    TEFFmpegVideoPacket *videoPacket = dynamic_cast<TEFFmpegVideoPacket *>(in_pIVideoPacket);
////    AVPacket *encVideoPkt = videoPacket->getFFmpegAVPacket();
////
////
////    AVRational timebase = {1,TE_TIME_BASE};
////#if !defined(__IOS__)
////    if (m_pInputFormatCtxV) {
////        AVStream *in_stream = m_pInputFormatCtxV->formatContext->streams[encVideoPkt->stream_index];
////        timebase = in_stream->time_base;
////    }
////#endif
////
////    // adjust dts and pts to output stream time base
////    encVideoPkt->stream_index = m_pVideoStream->index;
////    encVideoPkt->pts = av_rescale_q(encVideoPkt->pts, timebase, m_pVideoStream->time_base);
////    encVideoPkt->dts = av_rescale_q(encVideoPkt->dts, timebase, m_pVideoStream->time_base);
////    encVideoPkt->duration = av_rescale_q(encVideoPkt->duration, timebase, m_pVideoStream->time_base);
////
////    //    VELOGI("m_pVideoPacket->pts %" PRId64", in_stream->time_base.num %d, den %d,m_pVideoStream->time_base.num %d,
////    //    den %d",
////    //           encVideoPkt->pts, in_stream->time_base.num, in_stream->time_base.den,
////    //           m_pVideoStream->time_base.num,m_pVideoStream->time_base.den);
////
////    // adjust video start pts and dts from 0
////    if (m_bIsFirstRemuxVideo) {
////        if (encVideoPkt->pts != 0) {
////            VELOGW("video start pts in origin stream:%" PRId64, encVideoPkt->pts);
////            m_remuxVideoPtsStart = encVideoPkt->pts;
////        }
////        m_bIsFirstRemuxVideo = false;
////    }
////
////    encVideoPkt->pts -= m_remuxVideoPtsStart;
////    encVideoPkt->dts -= m_remuxVideoPtsStart;
////
////    encVideoPkt->pos = -1;
////    if ((encVideoPkt->flags & AV_PKT_FLAG_KEY)  && !m_videoEncodeExtradataReady) {
////        TEResult res = TER_OK;
////        AVCodecParameters *codecpar = m_pVideoStream->codecpar;
////        if (!codecpar->extradata) {
////            res = setupExtradataForStream(videoPacket);
////            if (res != TER_OK) {
////                return res;
////            }
////        }
////        m_videoEncodeExtradataReady = true;
////        if (!m_bFileHeaderReady) {
////            res = _writeAVFormatHeader();
////            if (res < 0) {
////                VELOGE("TEFFMpegFileWriter::writeVideoPacket, error occured in _writeAVFormatHeader ret = %d" ,res);
////                return res;
////            }
////        }
////    }
////
////    int64_t ret = writeAVPacketSafe(encVideoPkt, AVMEDIA_TYPE_VIDEO);
////    encVideoPkt = nullptr;
////
////    if (ret < 0) {
////        VELOGE("error in mux video packet, ret is %" PRId64, ret);
////    }
////    return ret;
////}
////// NOTE: !!!! As of 2020-8, iOS will not use TEFFMpegFileWriter to do any encoding !
////int64_t TEFFMpegFileWriter::_writeVideoFrame(ITEVideoFrame *in_pIVideoFrame, int64_t in_streamTime) {
////
////    int64_t ret;
////    int ffmpeg_ret = 0;
////    if (!isValid()) {
////        VELOGE("Format ctx not valid");
////        addReportSimpleErrorInfo("_writeVideoFrame() context", "output format context is null");
////        ret = COMBINE_I64ERROR((TER_INVALID_CONTEXT));
////        return ret;
////    }
////
////    if (!m_bStarted) {
////        VELOGE("Writer not started");
////        addReportSimpleErrorInfo("_writeVideoFrame() state", "Writer not started");
////        ret = COMBINE_I64ERROR((TER_INVALID_STATE));
////        return ret;
////    }
////
////    if (!m_pVideoStream) {
////        VELOGE("Video stream not ready");
////        addReportSimpleErrorInfo("_writeVideoFrame() m_pVideoStream", "m_pVideoStream is null");
////        ret = COMBINE_I64ERROR((TER_BAD_VIDEO_STREAM));
////        return ret;
////    }
////
////    if (!in_pIVideoFrame) {
////        //for video is null, we think it is not a error, but just discard current frame
////        VELOGE("Empty video frame");
////        addReportSimpleErrorInfo("_writeVideoFrame() frame", "Empty video frame");
////        ret = COMBINE_I64ERROR((TER_INVALID_PARAM));
////        return ret;
////    }
////
////    const ETEPixelFormat pixFmt = in_pIVideoFrame->getPixelFormat();
////
////#if defined(__IOS__)
////    if (pixFmt == TEPixFmt_None) {
////
////        return writeVideoPacket(in_pIVideoFrame, in_streamTime);
////    }
////    if (pixFmt != TEPixFmt_CVPixelBuffer) { // we currently only support pixelbuffer
////        VELOGE("Invalid pixel format of input image!");
////        ret = COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////        return ret;
////    }
////#else
////    if (TEPixelFormatToAVPixelFormat(pixFmt) != m_pVideoEncCtx->pix_fmt && AV_PIX_FMT_QSV != m_pVideoEncCtx->pix_fmt) {
////        if (m_pVideoEncCtx->pix_fmt != AV_PIX_FMT_CUDA || TEPixelFormatToAVPixelFormat(pixFmt) != AV_PIX_FMT_NV12) {
////            VELOGE("Invalid pixel format of input image! %d, %d, %d", TEPixelFormatToAVPixelFormat(pixFmt), m_pVideoEncCtx->pix_fmt, AV_PIX_FMT_NV12);
////            if (reportPtr != nullptr) {
////                char msg[100] = { 0 };
////                sprintf(msg, "invalid frame_format=%s", av_get_pix_fmt_name((AVPixelFormat)TEPixelFormatToAVPixelFormat(pixFmt)));
////                reportPtr->addErrorInfo("_writeVideoFrame() frame_format", msg);
////            }
////            ret = COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////            return ret;
////        }
////
////    }
////#endif
////
////    STEImagePosition imgPos {};
////    in_pIVideoFrame->getImagePosition(&imgPos);
////    if (!TERuntimeConfig::s_bEnableReEncodeOpt && ((int)imgPos.nImageWidth != m_pVideoEncCtx->width || (int)imgPos.nImageHeight != m_pVideoEncCtx->height)) {
////        VELOGE("Invalid input image size!， imgPos.nImageWidth: %d, m_pVideoEncCtx->width: %d, imgPos.nImageHeight: %d, m_pVideoEncCtx->height: %d", (int)imgPos.nImageWidth, m_pVideoEncCtx->width,
////               (int)imgPos.nImageHeight, m_pVideoEncCtx->height);
////        if (reportPtr != nullptr) {
////            char msg[100] = { 0 };
////            sprintf(msg, "invalid frame_size=(%dx%d)", (int)imgPos.nImageWidth, (int)imgPos.nImageHeight);
////            reportPtr->addErrorInfo("_writeVideoFrame() frame_size", msg);
////        }
////        ret = COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////        return ret;
////    }
////
////    STEImageBuffer imgBuffer{};
////    in_pIVideoFrame->getImageBuffer(&imgBuffer);
////
////    STEColorInfo colorInfo{};
////    in_pIVideoFrame->getColorInfo(colorInfo);
////
////#if defined(__IOS__)
////    int res = TECVPixelBuffer2AVFrame((CVPixelBufferRef)imgBuffer.data[0], m_pEncVideoFrame, colorInfo);
////
////    if (res != 0) {
////        return COMBINE_I64ERROR((TER_INVALID_PARAM));
////    }
////#else
////    // Setup image buffer pointer for each plane
////    if (in_pIVideoFrame->getMemoryLocation() == TEMemoryLocation_OpenGL_Texture || in_pIVideoFrame->getMemoryLocation() == TEMemoryLocation_OpenGL_TextureOES) {
////        STETextureFrameOpaque opaqueInfo{};
////        opaqueInfo.i64TextureID = in_pIVideoFrame->getHandle();
////        opaqueInfo.i64SharedEGLContext = 0;
////
////        if (!m_pEncVideoFrame->opaque) {
////            VELOGE("No frame texture, re alloc");
////            m_pEncVideoFrame->opaque = av_mallocz(sizeof(STETextureFrameOpaque));
////        }
////        memcpy(m_pEncVideoFrame->opaque, &opaqueInfo, sizeof(STETextureFrameOpaque));
////
////    }
////#ifdef _WIN32
////        else if (m_pVideoEncCtx->pix_fmt == AV_PIX_FMT_CUDA) {
////        auto src_frame = av_frame_alloc();
////
////        src_frame->width = m_pVideoEncCtx->width;
////        src_frame->height = m_pVideoEncCtx->height;
////        src_frame->format = in_pIVideoFrame->getMemoryLocation() == TEMemoryLocation_Host ? AV_PIX_FMT_NV12 : AV_PIX_FMT_CUDA;
////
////		const unsigned int planeCount = TEPlaneCountOfPixFmt(pixFmt);
////        assert(planeCount == 2);
////
////		for (unsigned int i = 0; i < planeCount; i++) {
////            src_frame->data[i] = (uint8_t*)imgBuffer.data[i];
////            src_frame->linesize[i] = imgBuffer.pitch[i];
////		}
////
////        av_frame_unref(m_pEncVideoFrame);
////        auto err = av_hwframe_get_buffer(m_pVideoEncCtx->hw_frames_ctx, m_pEncVideoFrame, 0);
////        if (err < 0) {
////            VELOGE("failed to get hw frame buffer");
////            return err;
////        }
////
////        if ((err = av_hwframe_transfer_data(m_pEncVideoFrame, src_frame, 0)) < 0) {
////			VELOGE("failed to transfer hw frame buffer");
////			return err;
////        }
////
////        av_frame_free(&src_frame);
////    } else if (m_pVideoEncCtx->pix_fmt == AV_PIX_FMT_QSV) {
////        av_frame_unref(m_pEncVideoFrame);
////        auto err = av_hwframe_get_buffer(m_pVideoEncCtx->hw_frames_ctx, m_pEncVideoFrame, 0);
////        if (err < 0) {
////            VELOGE("failed to get hw frame buffer");
////            return err;
////        }
////
////        mfxFrameSurface1* surface = (mfxFrameSurface1*)m_pEncVideoFrame->data[3];
////        if (m_ePixFmt == TEPixFmt_QSV_D3D11)
////        {
////            auto dst = reinterpret_cast<ID3D11Texture2D*>(surface->Data.MemId);
////            auto src = static_cast<TED3D11VideoFrame*>(in_pIVideoFrame)->getTexture2D();
////
////            auto frames_ctx = reinterpret_cast<AVHWFramesContext*>(m_pVideoEncCtx->hw_frames_ctx->data);
////            auto device_ctx = reinterpret_cast<AVHWDeviceContext *>(frames_ctx->device_ctx);
////
////            typedef struct QSVDevicePriv {
////                AVBufferRef* child_device_ctx;
////            } QSVDevicePriv;
////
////            QSVDevicePriv* priv = reinterpret_cast<QSVDevicePriv*>(device_ctx->user_opaque);
////            device_ctx = (AVHWDeviceContext*)priv->child_device_ctx->data;
////            auto d3d11dev_ctx = reinterpret_cast<AVD3D11VADeviceContext*>(device_ctx->hwctx);
////            ID3D11DeviceContext* context = d3d11dev_ctx->device_context;
////            context->CopySubresourceRegion(dst, 0, 0, 0, 0, src, 0, NULL);
////        }
////        else
////        {
////            TED3D9VideoFrame* frame = (TED3D9VideoFrame*)in_pIVideoFrame;
////            auto src = frame->getD3D9Surface();
////            if (!m_IntelVPP)
////                m_IntelVPP = std::unique_ptr<TEIntelVPP>(new TEIntelVPP(frame->getAllocator()));
////            m_IntelVPP->process(src, surface);
////        }
////    } else if (m_pVideoEncCtx->pix_fmt == AV_PIX_FMT_D3D11) {
////        av_frame_unref(m_pEncVideoFrame);
////        auto err = av_hwframe_get_buffer(m_pVideoEncCtx->hw_frames_ctx, m_pEncVideoFrame, 0);
////        if (err < 0) {
////            VELOGE("failed to get hw frame buffer");
////            return err;
////        }
////
////        auto dst = reinterpret_cast<ID3D11Texture2D*>(m_pEncVideoFrame->data[0]);
////        auto src = static_cast<TED3D11VideoFrame*>(in_pIVideoFrame)->getTexture2D();
////
////        auto device_ctx = reinterpret_cast<AVHWFramesContext*>(m_pVideoEncCtx->hw_frames_ctx->data);
////        ID3D11Device* device = reinterpret_cast<AVD3D11VADeviceContext*>(device_ctx->device_ctx->hwctx)->device;
////        ID3D11DeviceContext* context = reinterpret_cast<AVD3D11VADeviceContext*>(device_ctx->device_ctx->hwctx)->device_context;
////
////        D3D11_TEXTURE2D_DESC desc;
////        dst->GetDesc(&desc);
////
////        D3D11_TEXTURE2D_DESC dst_desc;
////        src->GetDesc(&dst_desc);
////
////        context->CopySubresourceRegion(dst, (UINT)m_pEncVideoFrame->data[1], 0, 0, 0, src, 0, NULL);
////    }
////#endif
////    else {
////        const unsigned int planeCount = TEPlaneCountOfPixFmt(pixFmt);
////        for (unsigned int i = 0; i < planeCount; i++) {
////            m_pEncVideoFrame->data[i] = (uint8_t *) imgBuffer.data[i];
////            m_pEncVideoFrame->linesize[i] = imgBuffer.pitch[i];
////        }
////        if (m_iFlags & TE_AV_FILE_WRITER_FLAG_GIF) {
////            //怀疑是ffmpeg bug
////            m_pEncVideoFrame->data[1] = m_pBgrData;
////        }
////    }
////#endif
////
////    if (TERuntimeConfig::s_bColorSpaceRepair) {
////        m_pEncVideoFrame->colorspace = (AVColorSpace)colorInfo.colorspace;
////        m_pEncVideoFrame->color_trc = (AVColorTransferCharacteristic)colorInfo.color_trc;
////        m_pEncVideoFrame->color_primaries = (AVColorPrimaries)colorInfo.color_primaries;
////        m_pEncVideoFrame->color_range = (AVColorRange)colorInfo.color_range;
////    }
////
////    m_pEncVideoFrame->width = m_pVideoEncCtx->width;
////    m_pEncVideoFrame->height = m_pVideoEncCtx->height;
////    m_pEncVideoFrame->format = m_pVideoEncCtx->pix_fmt;
////
////    m_pEncVideoFrame->key_frame = in_pIVideoFrame->getisKeyFrame();
////    m_iFrameNum++;
////    m_pEncVideoFrame->pts = av_rescale_q(in_streamTime, TE_TIME_BASE_Q, m_pVideoEncCtx->time_base);
////
////    if (m_i64LastPts == AV_NOPTS_VALUE && m_mediaCodecDecInstance) {
////        av_opt_set_int(m_pVideoEncCtx->priv_data, "mediaCodecDecInstance", reinterpret_cast<int64_t>(m_mediaCodecDecInstance), 0);
////    }
////
////    if (m_pEncVideoFrame->pts == m_i64LastPts) {
////        av_frame_unref(m_pEncVideoFrame);
////        if (m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER) {
////            //use malloc here, since it is a ffmpeg c based value
////            m_pEncVideoFrame->opaque = av_mallocz(sizeof(STETextureFrameOpaque));
////        }
////
////        if (reportPtr != nullptr) {
////            char msg[100] = { 0 };
////            sprintf(msg, "invalid frame_pts=%" PRId64, m_pEncVideoFrame->pts);
////            reportPtr->addErrorInfo("_writeVideoFrame() frame_pts", msg);
////        }
////        VELOGE("Timestamp repeat, in_streamTime:%" PRId64 ", pts:%" PRId64, in_streamTime, m_pEncVideoFrame->pts);
////        return COMBINE_I64ERROR((TER_BAD_TIME_STAMP));
////    }
////    m_i64LastPts = m_pEncVideoFrame->pts;
////
////    bool bNeedReSend = false;
////    do {
////        av_init_packet(m_pEncVideoPkt);
////
////        //send frame to encoder
////        ffmpeg_ret = avcodec_send_frame(m_pVideoEncCtx, m_pEncVideoFrame);
////        /*
////        * @return 0 on success, otherwise negative error code:
////        *      AVERROR(EAGAIN):   input is not accepted in the current state - user
////        *                         must read output with avcodec_receive_packet() (once
////        *                         all output is read, the packet should be resent, and
////        *                         the call will not fail with EAGAIN).
////        *      AVERROR_EOF:       the encoder has been flushed, and no new frames can
////        *                         be sent to it
////        *      AVERROR(EINVAL):   codec not opened, refcounted_frames not set, it is a
////        *                         decoder, or requires flush
////        *      AVERROR(ENOMEM):   failed to add packet to internal queue, or similar
////        *      other errors: legitimate decoding errors
////        */
////
////        if (ffmpeg_ret == AVERROR(EAGAIN)) {
////            bNeedReSend = true;   //frame need to be re-sent
////        } else if (ffmpeg_ret == 0) {
////            bNeedReSend = false;
////        } else if (ffmpeg_ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "avcodec_send_packet() video failed");
////            VELOGE("Timestamp repeat");
////            char errstr[256] = { 0 };
////            av_make_error_string(errstr, sizeof(errstr) - 1, ffmpeg_ret);
////            VELOGE("failed to send frame: %s", errstr);
////            addReportSimpleErrorInfo("writeVideoFrame avcodec_send_packet", errstr);
////            return COMBINE_I64ERROR(ffmpeg_ret);
////        }
////
////        ffmpeg_ret = avcodec_receive_packet(m_pVideoEncCtx, m_pEncVideoPkt);
////
////        /*
////        * @return 0 on success, otherwise negative error code:
////        *      AVERROR(EAGAIN):   output is not available in the current state - user
////        *                         must try to send input
////        *      AVERROR_EOF:       the encoder has been fully flushed, and there will be
////        *                         no more output packets
////        *      AVERROR(EINVAL):   codec not opened, or it is an encoder
////        *      other errors: legitimate decoding errors
////        */
////
////        if (ffmpeg_ret == AVERROR(EAGAIN)) {
////            break;
////        } else if (ffmpeg_ret == AVERROR_EOF) {
////            TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "avcodec_receive_packet() EOF");
////            return COMBINE_I64ERROR(ffmpeg_ret);
////        } else if (ffmpeg_ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "avcodec_receive_packet() failed");
////            addReportSimpleErrorInfo("writeVideoFrame avcodec_receive_packet < 0", "");
////            return COMBINE_I64ERROR(ffmpeg_ret);
////        }
////        m_bIsVideoPacketReceived = true;
////
////        m_pEncVideoPkt->stream_index = m_pVideoStream->index;
////        m_pEncVideoPkt->pts = av_rescale_q(m_pEncVideoPkt->pts - 0/*m_i64FisrtFrameEncodePts*/,
////                                           m_pVideoEncCtx->time_base, m_pVideoStream->time_base);
////        m_pEncVideoPkt->dts = av_rescale_q(m_pEncVideoPkt->dts - 0/*m_i64FisrtFrameEncodeDts*/,
////                                           m_pVideoEncCtx->time_base, m_pVideoStream->time_base);
////
////        m_pEncVideoPkt->duration = 0;//av_rescale_q(1, videoEncoderCtx->time_base, m_pVideoStream->time_base);
////#if defined(__IOS__)
////        m_pEncVideoPkt->duration = m_pVideoEncCtx->time_base.den * m_pVideoEncCtx->framerate.den / m_pVideoEncCtx->framerate.num;
////#endif
////
////        m_videoEncodeExtradataReady = true;
////        if (!m_bFileHeaderReady) {
////            int64_t result = _writeAVFormatHeader();
////            VELOGI("TEFFMpegFileWriter _writeAVFormatHeader from video packet result: %" PRId64, result);
////            if (result < 0) {
////                addReportSimpleErrorInfo("writeVideoFrame _writeAVFormatHeader fialed", "");
////                return result;
////            }
////        }
////
////        //        VELOGD("write video pkt, pts %" PRId64 ", dts %" PRId64, m_pEncVideoPkt->pts, m_pEncVideoPkt->dts);
////
////        if (m_pEncVideoPkt->pts < m_pEncVideoPkt->dts && m_pEncVideoPkt->dts != AV_NOPTS_VALUE && m_pEncVideoPkt->pts != AV_NOPTS_VALUE) {
////            auto min3 = [](auto x, auto y, auto z) noexcept { return std::min(std::min(x, y), z); };
////            auto max3 = [](auto x, auto y, auto z) noexcept { return std::max(std::max(x, y), z); };
////            m_pEncVideoPkt->pts = m_pEncVideoPkt->dts = m_pEncVideoPkt->pts + m_pEncVideoPkt->dts + m_i64LastDts + 1
////                                                        - min3(m_pEncVideoPkt->pts, m_pEncVideoPkt->dts, m_i64LastDts + 1)
////                                                        - max3(m_pEncVideoPkt->pts, m_pEncVideoPkt->dts, m_i64LastDts + 1);
////        }
////
////        m_i64LastDts = m_pEncVideoPkt->dts;
////
////        ffmpeg_ret = writeAVPacketSafe(m_pEncVideoPkt, AVMEDIA_TYPE_VIDEO);
////
////        //av_packet_unref(m_pEncVideoPkt);
////
////        if (ffmpeg_ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "av_interleaved_write_frame() failed");
////            return COMBINE_I64ERROR(ffmpeg_ret);
////        }
////
////        if (m_iFlags & TE_AV_FILE_WRITER_FLAG_REMUX_AUDIO) {
////            while (true) {
////                ffmpeg_ret = av_read_frame(m_pInputFormatCtx->formatContext, m_pEncAudioPkt);
////                if (ffmpeg_ret < 0) {
////                    TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "av_read_frame in remux Audio");
////                    break;
////                }
////
////                if (m_pEncAudioPkt->stream_index != m_nAudioStreamIndex) {
////                    av_packet_unref(m_pEncAudioPkt);
////                    continue;
////                }
////
////                AVStream *in_stream = m_pInputFormatCtx->formatContext->streams[m_pEncAudioPkt->stream_index];
////
////                m_pEncAudioPkt->stream_index = m_pAudioStreams[0]->index;
////                m_pEncAudioPkt->pts = av_rescale_q(m_pEncAudioPkt->pts, in_stream->time_base,
////                                                   m_pAudioStreams[0]->time_base);
////                m_pEncAudioPkt->dts = av_rescale_q(m_pEncAudioPkt->dts, in_stream->time_base,
////                                                   m_pAudioStreams[0]->time_base);
////                m_pEncAudioPkt->duration = av_rescale_q(m_pEncAudioPkt->duration,
////                                                        in_stream->time_base,
////                                                        m_pAudioStreams[0]->time_base);
////                m_pEncAudioPkt->pos = -1;
////
////                ffmpeg_ret = writeAVPacketSafe(m_pEncAudioPkt, AVMEDIA_TYPE_AUDIO);
////                if (ffmpeg_ret < 0) {
////                    TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "write pkt in remux Audio");
////                    break;
////                }
////
////                int64_t current = av_rescale_q(m_pEncAudioPkt->pts, m_pAudioStreams[0]->time_base,
////                                               TE_TIME_BASE_Q);
////
////                if (current >= in_streamTime)
////                    break;
////            }
////        }
////
////        ++m_iWriteVideoFrameCount;
////
////    } while (bNeedReSend);
////
////    return TER_OK;
////
////}
////
////int64_t TEFFMpegFileWriter::_writeAVFormatHeader() {
////    std::lock_guard<std::mutex> guard(m_writeLock);
////    if (!m_bFileHeaderReady) {
////        AVDictionary *opt = nullptr;
////        av_dict_set(&opt, "movflags", "use_metadata_tags", AV_DICT_MULTIKEY);
////#ifdef _WIN32
////        if (m_pVideoEncCtx && (m_pVideoEncCtx->codec->name == "h264_qsv"s || m_pVideoEncCtx->codec->name == "h264_amf"s)) {
////            av_dict_set(&opt, "movflags", "write_colr", AV_DICT_MULTIKEY);
////        }
////#endif
////        if (m_bIsEnableMoovFront) {
////            av_dict_set(&opt, "movflags", "faststart", AV_DICT_MULTIKEY);
////        }
////        // write video sps pps to extradata
////        if (m_pVideoStream && m_pVideoEncCtx && m_pVideoEncCtx->extradata && m_pVideoEncCtx->extradata_size > 0) {
////            VELOGI("TEFFMpegFileWriter write video extradata, size: %d", m_pVideoEncCtx->extradata_size);
////            if (m_pVideoStream->codecpar->extradata) {
////                av_free(m_pVideoStream->codecpar->extradata);
////            }
////            size_t paddingSize;
////#if defined(_WIN32)
////            paddingSize = AV_INPUT_BUFFER_PADDING_SIZE;
////#else
////            paddingSize = FF_INPUT_BUFFER_PADDING_SIZE;
////#endif
////            m_pVideoStream->codecpar->extradata = (uint8_t *)av_mallocz((size_t)m_pVideoEncCtx->extradata_size + paddingSize);
////            memcpy(m_pVideoStream->codecpar->extradata, m_pVideoEncCtx->extradata,
////                   (size_t)m_pVideoEncCtx->extradata_size);
////            m_pVideoStream->codecpar->extradata_size = m_pVideoEncCtx->extradata_size;
////        }
////        int ffmpeg_ret = avformat_write_header(m_pOutputFormatCtx, &opt);
////        av_dict_free(&opt);
////        if (ffmpeg_ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "TEFFMpegFileWriter _writeAVFormatHeader failed: %d", ffmpeg_ret);
////            if (m_pEncVideoPkt) {
////                av_packet_unref(m_pEncVideoPkt);
////            }
////
////            if (m_pEncAudioPkt) {
////                av_packet_unref(m_pEncAudioPkt);
////            }
////            cleanup();
////            return COMBINE_I64ERROR_WITH_EXT(ffmpeg_ret, AVFORMAT_WRITE_HEADER_FAILED);
////        }
////        m_bFileHeaderReady = true;
////#if defined(__ANDROID__)
////        VELOGI("TEFFMpegFileWriter _writeAVFormatHeader success, thread: %d", gettid());
////#endif
////    }
////    return TER_OK;
////}
////
////int64_t TEFFMpegFileWriter::writeAudioSamples(ITEAudioSamples *in_pIAudioSamplesArray[],
////                                              int64_t in_streamTimeArray[]) {
////    int64_t startAt = TETimeUtils::getCurrentTime();
////    int64_t ret;
////    if (in_pIAudioSamplesArray[0]) {
////        ret = _writeAudioSamples(in_pIAudioSamplesArray, in_streamTimeArray);
////    } else {
////        ret = _flushAudio();
////    }
////    m_iTotalWriteAudioTime += TETimeUtils::getCurrentTime() - startAt;
////
////    VELOGV("WriteAudioSample cost %" PRId64, TETimeUtils::getCurrentTime() - startAt);
////    return ret;
////}
////
////int64_t TEFFMpegFileWriter::_writeAudioSamples(ITEAudioSamples *in_pIAudioSamplesArray[],
////                                               int64_t in_streamTimeArray[]) {
////    int i, j;
////    int64_t ret;
////    int ffmpeg_ret = 0;
////
////    if (!isValid())
////        return COMBINE_I64ERROR((TER_INVALID_CONTEXT));
////
////    if (!m_bStarted)
////        return COMBINE_I64ERROR((TER_INVALID_STATE));
////
////    const int audioStreamCount = m_pAudioStreams.size();
////    if (!audioStreamCount)
////        return COMBINE_I64ERROR((TER_BAD_AUDIO_STREAM));
////
////    if (!m_pVideoStream && !m_bFileHeaderReady && m_pOutputFormatCtx) {   // means audio only
////        std::lock_guard<std::mutex> guard(m_writeLock);
////        if (!m_bFileHeaderReady) {   // after locked, check again
////            AVDictionary *opt = nullptr;
////            av_dict_set(&opt, "movflags", "use_metadata_tags", AV_DICT_MULTIKEY);
////            if (m_bIsEnableMoovFront) {
////                av_dict_set(&opt, "movflags", "faststart", AV_DICT_MULTIKEY);
////            }
////            ffmpeg_ret = avformat_write_header(m_pOutputFormatCtx, &opt);
////            av_dict_free(&opt);
////            if (ffmpeg_ret < 0) {
////                TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "TEFFMpegFileWriter _writeAudioSamples avformat_write_header failed");
////                return COMBINE_I64ERROR_WITH_EXT(ffmpeg_ret, AVFORMAT_WRITE_HEADER_FAILED);
////            }
////            m_bFileHeaderReady = true;
////            VELOGI("TEFFMpegFileWriter _writeAudioSamples audio write only header success");
////        }
////    }
////
////    if (audioStreamCount != m_pAudioEncCtx.size()) {
////        VELOGE("Wrong audio stream size");
////        return COMBINE_I64ERROR((TER_BAD_AUDIO_STREAM));
////    }
////
////    // Check input audio samples
////    for (i = 0; i < audioStreamCount; i++) {
////        ITEAudioSamples *pIAudioSamples = in_pIAudioSamplesArray[i];
////        if (!pIAudioSamples)
////            return COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////
////        if (TESampleFormatToAVSampleFormat(pIAudioSamples->getAudioSampleFormat()) !=
////            m_pAudioEncCtx[i]->sample_fmt) {
////            VELOGE("Invalid input audio sample format!");
////            return COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////        }
////        if (m_pAudioEncCtx[i]->frame_size && (pIAudioSamples->getActualSampleCount() !=
////                                              (unsigned int) m_pAudioEncCtx[i]->frame_size)) {
////            VELOGE("Invalid input audio sample frame size!");
////#if !defined(__IOS__)
////            return COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////#endif
////        }
////        if (pIAudioSamples->getChannelCount() != (unsigned int) m_pAudioEncCtx[i]->channels) {
////            VELOGE("Invalid input audio channel count!");
////            return COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////        }
////    }
////
////    //encode the audio frame
////    for (i = 0; i < audioStreamCount; i++) {
////
////        m_pEncAudioFrame->format = m_pAudioEncCtx[i]->sample_fmt;
////        m_pEncAudioFrame->nb_samples = in_pIAudioSamplesArray[i]->getActualSampleCount();
////        m_pEncAudioFrame->pts = av_rescale_q(in_streamTimeArray[i], TE_TIME_BASE_Q,
////                                             m_pAudioEncCtx[i]->time_base);
////        m_pEncAudioFrame->channels = in_pIAudioSamplesArray[i]->getChannelCount();
////
////        STEAudioSamplesBuffer sampleBuffer{};
////        in_pIAudioSamplesArray[i]->getAudioSamplesBuffer(&sampleBuffer);
////
////        if (TEIsPlanarAudioSmpFmt(in_pIAudioSamplesArray[i]->getAudioSampleFormat())) {
////            for (j = 0; j < m_pAudioEncCtx[i]->channels; j++) {
////                m_pEncAudioFrame->data[j] = (uint8_t *) sampleBuffer.data[j];
////            }
////        } else {
////            m_pEncAudioFrame->data[0] = (uint8_t *)sampleBuffer.data[0];
////        }
////        m_pEncAudioFrame->linesize[0] = sampleBuffer.nPlaneSizeInBytes;
////
////        bool bNeedReSend = false;
////        do {
////            av_init_packet(m_pEncAudioPkt);
////
////            //send frame to encoder
////            ffmpeg_ret = avcodec_send_frame(m_pAudioEncCtx[i], m_pEncAudioFrame);
////            /*
////            * @return 0 on success, otherwise negative error code:
////            *      AVERROR(EAGAIN):   input is not accepted in the current state - user
////            *                         must read output with avcodec_receive_packet() (once
////            *                         all output is read, the packet should be resent, and
////            *                         the call will not fail with EAGAIN).
////            *      AVERROR_EOF:       the encoder has been flushed, and no new frames can
////            *                         be sent to it
////            *      AVERROR(EINVAL):   codec not opened, refcounted_frames not set, it is a
////            *                         decoder, or requires flush
////            *      AVERROR(ENOMEM):   failed to add packet to internal queue, or similar
////            *      other errors: legitimate decoding errors
////            */
////
////            if (ffmpeg_ret == AVERROR(EAGAIN)) {
////                bNeedReSend = true;   //frame need to be re-sent
////            } else if (ffmpeg_ret == 0) {
////                bNeedReSend = false;    //send the frame successfully
////            } else if (ffmpeg_ret < 0) {
////                TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "TEFFMpegFileWriter _writeAudioSamples avcodec_send_packet() audio failed");
////                return COMBINE_I64ERROR(ffmpeg_ret);
////            }
////
////            ffmpeg_ret = avcodec_receive_packet(m_pAudioEncCtx[i], m_pEncAudioPkt);
////            /*
////            * @return 0 on success, otherwise negative error code:
////            *      AVERROR(EAGAIN):   output is not available in the current state - user
////            *                         must try to send input
////            *      AVERROR_EOF:       the encoder has been fully flushed, and there will be
////            *                         no more output packets
////            *      AVERROR(EINVAL):   codec not opened, or it is an encoder
////            *      other errors: legitimate decoding errors
////            */
////
////            if (ffmpeg_ret == AVERROR(EAGAIN)) {
////                break;
////            } else if (ffmpeg_ret == AVERROR_EOF) {
////                TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "TEFFMpegFileWriter _writeAudioSamples avcodec_receive_packet() EOF");
////                return COMBINE_I64ERROR(ffmpeg_ret);
////            } else if (ffmpeg_ret < 0) {
////                TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "TEFFMpegFileWriter _writeAudioSamples avcodec_receive_packet() failed");
////                return COMBINE_I64ERROR(ffmpeg_ret);
////            }
////
////            m_pEncAudioPkt->stream_index = m_pAudioStreams[i]->index;
////            m_pEncAudioPkt->pts = av_rescale_q(m_pEncAudioPkt->pts - 0/*m_i64FisrtFrameEncodePts*/,
////                                               m_pAudioEncCtx[i]->time_base,
////                                               m_pAudioStreams[i]->time_base);
////            m_pEncAudioPkt->dts = av_rescale_q(m_pEncAudioPkt->dts - 0/*m_i64FisrtFrameEncodeDts*/,
////                                               m_pAudioEncCtx[i]->time_base,
////                                               m_pAudioStreams[i]->time_base);
////            m_pEncAudioPkt->duration = 0;//av_rescale_q(1, videoEncoderCtx->time_base, m_pVideoStream->time_base);
////            ffmpeg_ret = writeAVPacketSafe(m_pEncAudioPkt, AVMEDIA_TYPE_AUDIO);
////
////            if (ffmpeg_ret < 0) {
////                TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "TEFFMpegFileWriter _writeAudioSamples writeAVPacketSafe failed");
////                return COMBINE_I64ERROR(ffmpeg_ret);
////            }
////            ++m_iWriteAudioFrameCount;
////
////        } while (bNeedReSend);
////    }
////    return TER_OK;
////}
////
////bool TEFFMpegFileWriter::haveAudioAsc(int index) {
////    if (index >= 0 || index < m_pAudioEncCtx.size()) {
////        AVCodecContext *codecContext = m_pAudioEncCtx[index];
////        if (codecContext->extradata_size > 0 && codecContext->extradata) {
////            return true;
////        }
////    }
////    return false;
////}
////
////int64_t TEFFMpegFileWriter::encodeAudioSamples(ITEAudioSamples *pIAudioSample, int64_t streamTime, ITEAudioSamples **ppAudioPackets, bool &sendAgain) {
////    int64_t startAt = TETimeUtils::getCurrentTime();
////    int64_t ffmpeg_ret;
////
////    if (!isValid()) {
////        addReportSimpleErrorInfo("encodeAudioSamples", "invalid context");
////        return COMBINE_I64ERROR((TER_INVALID_CONTEXT));
////    }
////    if (!m_bStarted) {
////        addReportSimpleErrorInfo("encodeAudioSamples", " not start");
////        return COMBINE_I64ERROR((TER_INVALID_STATE));
////    }
////    const int audioStreamCount = m_pAudioStreams.size();
////    if (audioStreamCount != 1 || audioStreamCount != m_pAudioEncCtx.size()) {
////        addReportSimpleErrorInfo("encodeAudioSamples", " bad audio stream");
////        return COMBINE_I64ERROR((TER_BAD_AUDIO_STREAM));
////    }
////
////    // only have one audio stream
////    int i = 0;
////    bool isFlushAudio = (pIAudioSample == nullptr);
////
////    if (!isFlushAudio) {
////        // Check input audio samples
////        ITEAudioSamples *pIAudioSamples = pIAudioSample;
////        if (TESampleFormatToAVSampleFormat(pIAudioSamples->getAudioSampleFormat()) != m_pAudioEncCtx[i]->sample_fmt) {
////            VELOGE("Invalid input audio sample format!");
////            addReportSimpleErrorInfo("encodeAudioSamples", "Invalid input audio sample format");
////            return COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////        }
////        if (m_pAudioEncCtx[i]->frame_size
////            && (pIAudioSamples->getActualSampleCount() != (unsigned int)m_pAudioEncCtx[i]->frame_size)) {
////            VELOGE("Invalid input audio sample frame size!");
////            addReportSimpleErrorInfo("encodeAudioSamples", "Invalid input audio sample frame size");
////            return COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////        }
////        if (pIAudioSamples->getChannelCount() != (unsigned int)m_pAudioEncCtx[i]->channels) {
////            VELOGE("Invalid input audio channel count!");
////            addReportSimpleErrorInfo("encodeAudioSamples", "Invalid input audio sample channel");
////            return COMBINE_I64ERROR((TER_INVALID_FRAME_FORMAT));
////        }
////
////        // encode the audio frame
////        m_pEncAudioFrame->format = m_pAudioEncCtx[i]->sample_fmt;
////        m_pEncAudioFrame->nb_samples = pIAudioSample->getActualSampleCount();
////        m_pEncAudioFrame->pts = av_rescale_q(streamTime, TE_TIME_BASE_Q, m_pAudioEncCtx[i]->time_base);
////
////        STEAudioSamplesBuffer sampleBuffer {};
////        pIAudioSample->getAudioSamplesBuffer(&sampleBuffer);
////
////        if (TEIsPlanarAudioSmpFmt(pIAudioSample->getAudioSampleFormat())) {
////            for (int j = 0; j < m_pAudioEncCtx[i]->channels; j++) {
////                m_pEncAudioFrame->data[j] = (uint8_t *)sampleBuffer.data[j];
////            }
////        } else {
////            m_pEncAudioFrame->data[0] = (uint8_t *)sampleBuffer.data[0];
////        }
////        m_pEncAudioFrame->linesize[0] = sampleBuffer.nPlaneSizeInBytes;
////    } else {
////        // send NULL frame(flush frame) to encoder
////        m_pEncAudioFrame = nullptr;
////    }
////
////    // send frame to encoder
////    ffmpeg_ret = avcodec_send_frame(m_pAudioEncCtx[i], m_pEncAudioFrame);
////    /*
////     * @return 0 on success, otherwise negative error code:
////     *      AVERROR(EAGAIN):   input is not accepted in the current state - user
////     *                         must read output with avcodec_receive_packet() (once
////     *                         all output is read, the packet should be resent, and
////     *                         the call will not fail with EAGAIN).
////     *      AVERROR_EOF:       the encoder has been flushed, and no new frames can
////     *                         be sent to it
////     *      AVERROR(EINVAL):   codec not opened, refcounted_frames not set, it is a
////     *                         decoder, or requires flush
////     *      AVERROR(ENOMEM):   failed to add packet to internal queue, or similar
////     *      other errors: legitimate decoding errors
////     */
////
////    if (ffmpeg_ret == AVERROR(EAGAIN)) {
////        sendAgain = true;
////        TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "avcodec_send_packet() again");
////    } else if (ffmpeg_ret < 0) {
////        TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "avcodec_send_packet() failed");
////        addReportSimpleErrorInfo("encodeAudioSamples", "avcodec_send_packet() failed");
////        return COMBINE_I64ERROR(ffmpeg_ret);
////    } else {
////        sendAgain = false;
////    }
////
////    AVPacket *encAudioPkt = (dynamic_cast<TEFFmpegAudioPackets *>(*ppAudioPackets))->getFFmpegAVPacket();
////    av_init_packet(encAudioPkt);
////    ffmpeg_ret = avcodec_receive_packet(m_pAudioEncCtx[i], encAudioPkt);
////    /*
////     * @return 0 on success, otherwise negative error code:
////     *      AVERROR(EAGAIN):   output is not available in the current state - user
////     *                         must try to send input
////     *      AVERROR_EOF:       the encoder has been fully flushed, and there will be
////     *                         no more output packets
////     *      AVERROR(EINVAL):   codec not opened, or it is an encoder
////     *      other errors: legitimate decoding errors
////     */
////
////    if (ffmpeg_ret == AVERROR(EAGAIN)) {
////        return COMBINE_I64ERROR(ffmpeg_ret);
////    } else if (ffmpeg_ret == AVERROR_EOF) {
////        TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "avcodec_receive_packet() EOF");
////        return COMBINE_I64ERROR(ffmpeg_ret);
////    } else if (ffmpeg_ret < 0) {
////        TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "avcodec_receive_packet() failed");
////        return COMBINE_I64ERROR(ffmpeg_ret);
////    }
////
////    encAudioPkt->stream_index = m_pAudioStreams[i]->index;
////    encAudioPkt->pts = av_rescale_q(encAudioPkt->pts - 0 /*m_i64FisrtFrameEncodePts*/, m_pAudioEncCtx[i]->time_base,
////                                    m_pAudioStreams[i]->time_base);
////    encAudioPkt->dts = av_rescale_q(encAudioPkt->dts - 0 /*m_i64FisrtFrameEncodeDts*/, m_pAudioEncCtx[i]->time_base,
////                                    m_pAudioStreams[i]->time_base);
////    encAudioPkt->duration = 0;   // av_rescale_q(1, videoEncoderCtx->time_base, m_pVideoStream->time_base);
////
////    if (isFlushAudio) {
////        m_iTotalWriteAudioTime += TETimeUtils::getCurrentTime() - startAt;
////        VELOGV("WriteAudioSample cost %" PRId64, TETimeUtils::getCurrentTime() - startAt);
////    }
////
////    return TER_OK;
////}
////
////int64_t TEFFMpegFileWriter::muxAudioPackets(ITEAudioSamples *pAudioPackets, bool needUnRef) {
////    int64_t ret = 0;
////
////    if (!m_pVideoStream && !m_bFileHeaderReady && m_pOutputFormatCtx) {   // means audio only
////        std::lock_guard<std::mutex> guard(m_writeLock);
////        if (!m_bFileHeaderReady) {   // after locked, check again
////            AVDictionary *opt = nullptr;
////            av_dict_set(&opt, "movflags", "use_metadata_tags", AV_DICT_MULTIKEY);
////            if (m_bIsEnableMoovFront) {
////                av_dict_set(&opt, "movflags", "faststart", AV_DICT_MULTIKEY);
////            }
////            ret = avformat_write_header(m_pOutputFormatCtx, &opt);
////            av_dict_free(&opt);
////            if (ret < 0) {
////                TELogcat::LogFFMpegError(TAG, ret, "TEFFMpegFileWriter muxAudioPackets avformat_write_header failed");
////                return COMBINE_I64ERROR_WITH_EXT(ret, AVFORMAT_WRITE_HEADER_FAILED);
////            }
////            m_bFileHeaderReady = true;
////            VELOGI("TEFFMpegFileWriter muxAudioPackets audio write only header success");
////        }
////    }
////
////    std::lock_guard<std::mutex> guard(m_writeLock);
////
////    if (m_bFileHeaderReady && !m_listAudioPkt.empty()) {   // 只要 m_bFileHeaderReady 为 true，尽可能flush
////        for (auto pPkt : m_listAudioPkt) {
////            ret = av_interleaved_write_frame(m_pOutputFormatCtx, pPkt);
////            if (ret < 0) {
////                TELogcat::LogFFMpegError(
////                        TAG, ret, "TEFFMpegFileWriter muxAudioPackets av_interleaved_write_frame() audio failed 1");
////            }
////            av_packet_free(&pPkt);
////        }
////        m_listAudioPkt.clear();
////    }
////
////    AVPacket *encAudioPkt = (dynamic_cast<TEFFmpegAudioPackets *>(pAudioPackets))->getFFmpegAVPacket();
////    if (!m_bFileHeaderReady) {
////        AVPacket *pkt_new = av_packet_alloc();
////        if (needUnRef) {
////            av_packet_move_ref(pkt_new, encAudioPkt);
////        } else {
////            av_packet_ref(pkt_new, encAudioPkt);
////        }
////        if (pkt_new->pts < 0) {
////            VELOGE("TEFFMpegFileWriter muxAudioPackets, cache audio packet , pts: %" PRId64
////            ", data size: %d, list size: %zu",
////                    pkt_new->pts, pkt_new->size, m_listAudioPkt.size());
////        }
////        m_listAudioPkt.push_back(pkt_new);
////    } else {
////        // av_interleaved_write_frame will unref pkt internal, so ref encAudioPkt before
////        av_packet_ref(m_pEncAudioPkt, encAudioPkt);
////        ret = av_interleaved_write_frame(m_pOutputFormatCtx, m_pEncAudioPkt);
////        if (ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ret,
////                                     "TEFFMpegFileWriter muxAudioPackets av_interleaved_write_frame() audio failed 2");
////        }
////        if (encAudioPkt != nullptr && needUnRef) {
////            av_packet_unref(encAudioPkt);
////        }
////    }
////
////    return ret;
////}
////
////int64_t TEFFMpegFileWriter::flushWriter() {
////    if (!isValid()) {
////        addReportSimpleErrorInfo("flushWriter", "invalid context");
////        return TER_INVALID_CONTEXT;
////    }
////
////    if (!m_bStarted) {
////        addReportSimpleErrorInfo("flushWriter", "not start");
////        return TER_INVALID_STATE;
////    }
////
////    if (!(TE_AV_FILE_WRITER_FLAG_REMUX_VIDEO & m_iFlags)) {
////        if (TER_OK != flushVideo()) {
////            //if flush video failed, we just need a log
////            VELOGE("%s, %d", __FUNCTION__, __LINE__);
////        }
////    }
////
////    if (!m_bFileHeaderReady) {
////        // the last attempt
////        int64_t result = _writeAVFormatHeader();
////        VELOGI("TEFFMpegFileWriter _writeAVFormatHeader from flushWriter result: %" PRId64, result);
////        if (result < 0) {
////            return result;
////        }
////    }
////
////    if (!(TE_AV_FILE_WRITER_FLAG_REMUX_AUDIO & m_iFlags)) {
////        if (TER_OK != flushAudio()) {
////            //if flush audio failed, we just need a log
////            VELOGE("%s, %d", __FUNCTION__, __LINE__);
////        }
////    }
////
////    // Write the trailer, if any. The trailer must be written before you
////    // close the Codec Contexts opened when you wrote the header; otherwise
////    // av_write_trailer() may try to use memory that was freed on av_codec_close().
////    int ffmpeg_ret;
////    if (m_bFileHeaderReady) {
////        if (m_pTEIOBufCtx) {
////            m_pTEIOBufCtx->readytoWriteTrailer = true;
////        }
////
////#ifdef IS64BIT
////        // 64位下av_write_trailer 会改变m_pTEIOBufCtx->sendBufFunc的地址导致崩溃，
////        // 这里先做个workaround，临时保存sendBufFunc，write trailer之后再赋值回去
////        TE_IO_SEND_BUF sendBuf = nullptr;
////        if (m_pTEIOBufCtx) {
////            sendBuf = m_pTEIOBufCtx->sendBufFunc;
////        }
////#endif
////        if(m_bIsCompileSoftInfo) {
////            av_log_set_level(AV_LOG_INFO);
////            av_log_set_callback(custom_log_callback);
////
////#if !defined(__IOS__)
////            if( TECompileProbe::mParserEncodeInfo == nullptr )
////                TECompileProbe::mParserEncodeInfo = new TEParserEncodeInfo();
////#endif
////
////        }
////        ffmpeg_ret = av_write_trailer(m_pOutputFormatCtx);
////
////        if (m_pTEIOBufCtx) {
////#ifdef IS64BIT
////            m_pTEIOBufCtx->sendBufFunc = sendBuf;
////#endif
////        }
////
////        if (ffmpeg_ret) {
////            TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "av_write_trailer() failed");
////            cleanup();
////            ffmpeg_ret = TEErrorUtils::FFmpegError2TEError(ffmpeg_ret);
////            return COMBINE_I64ERROR_WITH_EXT(ffmpeg_ret,AV_WRITE_TRAILER_FAILED);
////        }
////        m_bFileHeaderReady = false;
////    }
////    VELOGD("%s, %d", __FUNCTION__, __LINE__);
////    cleanup();
////
////    if(m_bIsCompileSoftInfo) {
////        updateEditorInfo();
////    }
////
////    if (m_pTEIOBufCtx) {        // 最后上传边合成边上传的文件头和文件尾，保证业务层给客户端设置有效的编码信息
////        _flushIOBufCache();
////    }
////
////    if (reportPtr != nullptr) {
////        reportPtr->removeErrorInfo(FLUSH_WRITER);
////    }
////    VELOGD("TEFFMpegFileWriter finished");
////    return TER_OK;
////}
////
////void TEFFMpegFileWriter::updateEditorInfo() {
////#if defined(__ANDROID__)
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_I().frame_count >= 0 )
////        TEEditorInfo::eidtorinfoInt("te_composition_i_frame_count", TECompileProbe::mParserEncodeInfo->getFrameInfo_I().frame_count);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_I().frame_qp >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_i_frame_qp", TECompileProbe::mParserEncodeInfo->getFrameInfo_I().frame_qp);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_I().frame_size >= 0 )
////        TEEditorInfo::eidtorinfoInt("te_composition_i_frame_size", TECompileProbe::mParserEncodeInfo->getFrameInfo_I().frame_size);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_I().g_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_i_frame_g_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_I().g_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_I().y_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_i_frame_y_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_I().y_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_I().u_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_i_frame_u_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_I().u_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_I().v_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_i_frame_v_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_I().v_psnr);
////
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_P().frame_count >= 0 )
////        TEEditorInfo::eidtorinfoInt("te_composition_p_frame_count", TECompileProbe::mParserEncodeInfo->getFrameInfo_P().frame_count);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_P().frame_qp >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_p_frame_qp", TECompileProbe::mParserEncodeInfo->getFrameInfo_P().frame_qp);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_P().frame_size >= 0 )
////        TEEditorInfo::eidtorinfoInt("te_composition_p_frame_size", TECompileProbe::mParserEncodeInfo->getFrameInfo_P().frame_size);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_P().g_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_p_frame_g_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_P().g_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_P().y_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_p_frame_y_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_P().y_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_P().u_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_p_frame_u_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_P().u_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_P().v_psnr >=0.0f  )
////        TEEditorInfo::eidtorinfoFloat("te_composition_p_frame_v_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_P().v_psnr);
////
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_B().frame_count >= 0 )
////        TEEditorInfo::eidtorinfoInt("te_composition_b_frame_count", TECompileProbe::mParserEncodeInfo->getFrameInfo_B().frame_count);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_B().frame_qp >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_b_frame_qp", TECompileProbe::mParserEncodeInfo->getFrameInfo_B().frame_qp);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_B().frame_size >= 0 )
////        TEEditorInfo::eidtorinfoInt("te_composition_b_frame_size", TECompileProbe::mParserEncodeInfo->getFrameInfo_B().frame_size);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_B().g_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_b_frame_g_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_B().g_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_B().y_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_b_frame_y_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_B().y_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_B().u_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_b_frame_u_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_B().u_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getFrameInfo_B().v_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_b_frame_v_psnr", TECompileProbe::mParserEncodeInfo->getFrameInfo_B().v_psnr);
////
////    if( TECompileProbe::mParserEncodeInfo->getmbInfo_P().i_mb >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_p_frame_i_mb", TECompileProbe::mParserEncodeInfo->getmbInfo_P().i_mb);
////    if( TECompileProbe::mParserEncodeInfo->getmbInfo_P().skip >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_p_frame_skip", TECompileProbe::mParserEncodeInfo->getmbInfo_P().skip);
////    if( TECompileProbe::mParserEncodeInfo->getmbInfo_B().i_mb >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_b_frame_i_mb", TECompileProbe::mParserEncodeInfo->getmbInfo_B().i_mb);
////    if( TECompileProbe::mParserEncodeInfo->getmbInfo_B().skip >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_b_frame_skip", TECompileProbe::mParserEncodeInfo->getmbInfo_B().skip);
////    if( TECompileProbe::mParserEncodeInfo->getmbInfo_B().direct >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_b_frame_direct", TECompileProbe::mParserEncodeInfo->getmbInfo_B().direct);
////
////    if( TECompileProbe::mParserEncodeInfo->getPSNRInfo().video_bitrate >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_video_bitrate", TECompileProbe::mParserEncodeInfo->getPSNRInfo().video_bitrate);
////    if( TECompileProbe::mParserEncodeInfo->getPSNRInfo().y_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_y_psnr", TECompileProbe::mParserEncodeInfo->getPSNRInfo().y_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getPSNRInfo().u_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_u_psnr", TECompileProbe::mParserEncodeInfo->getPSNRInfo().u_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getPSNRInfo().v_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_v_psnr", TECompileProbe::mParserEncodeInfo->getPSNRInfo().v_psnr);
////    if( TECompileProbe::mParserEncodeInfo->getPSNRInfo().g_psnr >= 0.0f )
////        TEEditorInfo::eidtorinfoFloat("te_composition_g_psnr", TECompileProbe::mParserEncodeInfo->getPSNRInfo().g_psnr);
////
////#endif
////}
////
////int64_t TEFFMpegFileWriter::addVideoStreamForMux(TEBundle &muxParams) {
////    STEVideoResolution in_videoRes = *static_cast<STEVideoResolution *>(muxParams.getHandle(VIDEO_ENCODE_RESOLUTION));
////    STERational in_avgFps = *static_cast<STERational *>(muxParams.getHandle(VIDEO_ENCODE_AVERAGE_FPS));
////
////    // Create video stream
////    const char *key = "COMPILE_CODEC_ID";
////    if (muxParams.containsKey(key)) {
////        m_iCodecID = muxParams.getInt(key);
////    }
////    m_pVideoStream = avformat_new_stream(m_pOutputFormatCtx, NULL);
////    if (!m_pVideoStream) {
////        VELOGE("Failed to add video stream!");
////        return AVERROR(ENOMEM);
////    }
////
////    // Stream id
////    m_pVideoStream->id = m_pOutputFormatCtx->nb_streams - 1;
////
////    // Pixel aspect ratio
////    if (!in_videoRes.sImagePAR.num || !in_videoRes.sImagePAR.den) {
////        VELOGE("Invalid pixel aspect ratio!");
////        cleanup();
////        return TER_UNSUPPORTED;
////    }
////
////    //
////    // setup video stream
////    //
////    AVDictionary *opt = nullptr;
////    int framerate;
////    if(in_avgFps.num > 0) {
////        // Set timebase (1/framerate) used by encoder
////        framerate = in_avgFps.num / in_avgFps.den;
////        if (framerate < 5) {
////            framerate = 7;
////        }
////
////        m_pVideoStream->avg_frame_rate = {framerate, 1};
////    }else {
////        framerate = 25;
////        m_pVideoStream->avg_frame_rate = {framerate, 1};
////    }
////
////    m_pVideoStream->time_base = {1, TE_TIME_BASE};
////    m_iAssumedVPktDuration = av_rescale_q(1, {1,framerate}, m_pVideoStream->time_base);
////
////    m_pVideoStream->codecpar->codec_id = (AVCodecID)m_iCodecID;
////    m_pVideoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
////    if (m_iCodecID == AV_CODEC_ID_H264) {
////        m_pVideoStream->codecpar->codec_tag = MKTAG('a', 'v', 'c', '1');
////    } else if (m_iCodecID == AV_CODEC_ID_BYTE_VC1) {
////        m_bProfileIsBaseline = false;
////        m_pVideoStream->codecpar->codec_tag = MKTAG('h', 'v', 'c', '1');
////    }
////    m_pVideoStream->codecpar->width = in_videoRes.nImageWidth;
////    m_pVideoStream->codecpar->height = in_videoRes.nImageHeight;
////    m_pVideoStream->codecpar->color_trc = AVCOL_TRC_BT709;
////
////    if (TERuntimeConfig::s_bColorSpaceRepair) { // android use FFMpeg symbols directly, which conflict with avfoundation. so on ios, we use vtconfig data to specify color primaries/ transfer func/ ycrcb matrix
////        AVColorSpace colorSpace = (AVColorSpace)muxParams.getInt(COMPILE_COLOR_SPACE);
////        if (AVCOL_SPC_BT2020_NCL == colorSpace) {
////            m_pVideoStream->codecpar->color_primaries = AVCOL_PRI_BT2020;
////            m_pVideoStream->codecpar->color_trc = AVCOL_TRC_BT2020_10;
////            m_pVideoStream->codecpar->color_space = AVCOL_SPC_BT2020_NCL;
////        } else if (AVCOL_SPC_BT709 == colorSpace) {
////            m_pVideoStream->codecpar->color_primaries = AVCOL_PRI_BT709;
////            m_pVideoStream->codecpar->color_trc = AVCOL_TRC_BT709; // used to determine transfer func
////            m_pVideoStream->codecpar->color_space = AVCOL_SPC_BT709; // used to determine ycrcbmatrix
////        } else if (AVCOL_SPC_SMPTE170M == colorSpace) {
////            m_pVideoStream->codecpar->color_primaries = AVCOL_PRI_SMPTE170M;
////            m_pVideoStream->codecpar->color_trc = AVCOL_TRC_SMPTE170M;
////            m_pVideoStream->codecpar->color_space = AVCOL_SPC_SMPTE170M;
////        } else {
////            m_pVideoStream->codecpar->color_primaries = AVCOL_PRI_UNSPECIFIED;
////            m_pVideoStream->codecpar->color_trc = AVCOL_TRC_UNSPECIFIED;
////            m_pVideoStream->codecpar->color_space = AVCOL_SPC_UNSPECIFIED;
////        }
////
////        m_pVideoStream->codecpar->color_range = (AVColorRange)muxParams.getInt(COMPILE_COLOR_RANGE);
////    }
////
////    // Image size
////    m_pVideoStream->codecpar->width = (int) in_videoRes.nImageWidth;
////    m_pVideoStream->codecpar->height = (int) in_videoRes.nImageHeight;
////
////#if defined(__IOS__)
////    m_pVideoStream->codecpar->format = AV_PIX_FMT_YUV420P;//(AVPixelFormat)TEPixelFormatToAVPixelFormat(in_inputImagePixelFormat);
////    m_pVideoStream->codecpar->sample_aspect_ratio = m_pVideoStream->sample_aspect_ratio;
////#else // else defined(__IOS__)
////
////#endif // end of defined(__IOS__)
////
////
////    //Setting video rotation
////    string strRotation;
////    switch (in_videoRes.eImageRotation) {
////        case TEVideoRotation_0:
////            strRotation = "0";
////            break;
////        case TEVideoRotation_90:
////            strRotation = "90";
////            break;
////        case TEVideoRotation_180:
////            strRotation = "180";
////            break;
////        case TEVideoRotation_270:
////            strRotation = "270";
////            break;
////        default:
////            strRotation = "0";
////            break;
////    }
////    int ret = av_dict_set(&m_pVideoStream->metadata, "rotate", strRotation.c_str(), 0);
////#if defined(__IOS__)
////    av_dict_set(&m_pVideoStream->metadata, "creation_time", m_creationDate.c_str(), 0);
////#endif
////
////    if (ret) {
////        VELOGE("YXFFMpegFileWriter::addVideoStream(): set rotation angle failed");
////    }
////
////    return TER_OK;
////}
////
////TEResult TEFFMpegFileWriter::setupExtradataForStream(TEFFmpegVideoPacket *pkt)
////{
////    TEResult status = TER_OK;
////    AVCodecParameters *codecpar = m_pVideoStream->codecpar;
////    int extraSize = 0;
////    uint8_t *extraData = (uint8_t *)pkt->getExtraData(extraSize);
////    if (extraData && extraSize > 0) {
////        codecpar->extradata = (uint8_t *)av_mallocz(extraSize);
////        memmove(codecpar->extradata, extraData, extraSize);
////        codecpar->extradata_size = extraSize;
////
////    } else {
////        VELOGE("TEFFMpegFileWriter::setupExtradataForStream extradata not exist!");
////        status = TER_INVALID_PARAM;
////    }
////    m_bProfileIsBaseline = pkt->getIsBaseline();
////    return status;
////}
////
////int64_t TEFFMpegFileWriter::flushVideo() {
////    int64_t startAt = TETimeUtils::getCurrentTime();
////    int64_t ret = _flushVideo();
////    m_iTotalWriteVideoTime += TETimeUtils::getCurrentTime() - startAt;
////    VELOGV("TEFFMpegFileWriter flushVideo cost %" PRId64, TETimeUtils::getCurrentTime() - startAt);
////
////    return ret;
////}
////
////int64_t TEFFMpegFileWriter::_flushVideo() {
////
////    int ffmpeg_ret;
////    bool bNeedSend = true;
////
////    if (!m_pVideoStream)
////        return COMBINE_I64ERROR((TER_BAD_VIDEO_STREAM));
////
////    if (VIDEO_WRITING_ENCODE_AND_MUX == m_writeMode) {
////        int maxTryCount = 10, tryCount = 0;
////        while (true) {
////            if (bNeedSend || (m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER)) {
////                //send NULL frame(flush frame) to encoder
////                if (!(m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER)) {
////                    ffmpeg_ret = avcodec_send_frame(m_pVideoEncCtx, nullptr);
////                } else {
////#ifdef _WIN32
////                    ffmpeg_ret = avcodec_send_frame(m_pVideoEncCtx, nullptr);
////#else
////                    // Allocate AV frame structure to fill input image information
////                    AVFrame *tmpFrame = av_frame_alloc();
////                    tmpFrame->opaque = av_mallocz(sizeof(STETextureFrameOpaque));
////                    tmpFrame->width = m_pVideoEncCtx->width;
////                    tmpFrame->height = m_pVideoEncCtx->height;
////                    tmpFrame->format = m_pVideoEncCtx->pix_fmt;
////                    tmpFrame->key_frame = false;
////                    ffmpeg_ret = avcodec_send_frame(m_pVideoEncCtx, tmpFrame);
////
////                    if (tmpFrame->opaque)
////                        av_free(tmpFrame->opaque);
////                    av_frame_free(&tmpFrame);
////#endif
////                }
////
////                if (ffmpeg_ret == AVERROR(EAGAIN)) {
////                    bNeedSend = true;   //frame need to be re-sent
////                } else if (ffmpeg_ret == 0) {
////                    bNeedSend = false;    //send the frame successfully
////                } else if (ffmpeg_ret < 0) {
////                    TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "avcodec_send_packet() failed");
////                    return COMBINE_I64ERROR(ffmpeg_ret);
////                }
////            }
////
////            av_init_packet(m_pEncVideoPkt);
////            ffmpeg_ret = avcodec_receive_packet(m_pVideoEncCtx, m_pEncVideoPkt);
////
////            if (ffmpeg_ret == AVERROR(EAGAIN)) {
////                if (!m_bIsVideoPacketReceived && tryCount < maxTryCount) {
////                    tryCount++;
////                    VELOGW("try resend and receive first video packet, bNeedSend %d, tryCount %d", bNeedSend, tryCount);
////                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
////                    continue;
////                } else {
////                    break;
////                }
////            } else if (ffmpeg_ret == AVERROR_EOF) {
////                return TER_OK;
////            } else if (ffmpeg_ret < 0) {
////                TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "avcodec_receive_packet() failed");
////                return COMBINE_I64ERROR(ffmpeg_ret);
////            }
////            m_bIsVideoPacketReceived = true;
////
////            if (!m_bFileHeaderReady) {
////                // the last attempt
////                int64_t result = _writeAVFormatHeader();
////                VELOGI("TEFFMpegFileWriter _writeAVFormatHeader from _flushVideo result: %" PRId64, result);
////                if (result < 0) {
////                    return result;
////                }
////            }
////
////            m_pEncVideoPkt->stream_index = m_pVideoStream->index;
////            m_pEncVideoPkt->pts = av_rescale_q(m_pEncVideoPkt->pts - 0 /*m_i64FisrtFrameEncodePts*/, m_pVideoEncCtx->time_base, m_pVideoStream->time_base);
////            m_pEncVideoPkt->dts = av_rescale_q(m_pEncVideoPkt->dts - 0 /*m_i64FisrtFrameEncodeDts*/, m_pVideoEncCtx->time_base, m_pVideoStream->time_base);
////            m_pEncVideoPkt->duration = 0;   // av_rescale_q(1, videoEncoderCtx->time_base, m_pVideoStream->time_base);
////#if defined(__IOS__)
////            m_pEncVideoPkt->duration = m_pVideoEncCtx->time_base.den * m_pVideoEncCtx->framerate.den / m_pVideoEncCtx->framerate.num;
////#endif
////            VELOGD("TEFFMpegFileWriter flush video write pkt, size %d, %" PRId64 ", %" PRId64, m_pEncVideoPkt->size, m_pEncVideoPkt->pts, m_pEncVideoPkt->dts);
////            ffmpeg_ret = writeAVPacketSafe(m_pEncVideoPkt, AVMEDIA_TYPE_VIDEO);
////            //av_packet_unref(m_pEncVideoPkt);
////
////            if (ffmpeg_ret < 0) {
////                TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "av_interleaved_write_frame() failed");
////                return COMBINE_I64ERROR(ffmpeg_ret);
////            }
////            ++m_iWriteVideoFrameCount;
////        }
////    }
////
////    //flush pkt in queue
////    ffmpeg_ret = writeAVPacketSafe(nullptr, AVMEDIA_TYPE_VIDEO);
////    if (ffmpeg_ret < 0) {
////        TELogcat::LogFFMpegError(TAG, ffmpeg_ret, "writeAVPacketSafe null audio failed");
////        return COMBINE_I64ERROR(ffmpeg_ret);
////    }
////
////    return TER_OK;
////}
////
////int64_t TEFFMpegFileWriter::flushAudio() {
////    int64_t startAt = TETimeUtils::getCurrentTime();
////    int64_t ret = _flushAudio();
////    m_iTotalWriteAudioTime += TETimeUtils::getCurrentTime() - startAt;
////    VELOGV("TEFFMpegFileWriter flushAudio cost %" PRId64, TETimeUtils::getCurrentTime() - startAt);
////
////    return ret;
////}
////
////int64_t TEFFMpegFileWriter::_flushAudio() {
////    int ret;
////    int i;
////    bool bNeedSend = true;
////
////    const int audioStreamCount = m_pAudioStreams.size();
////
////    if (m_iWriteAudioFrameCount <= 0)
////        return TER_OK;
////
////    for (i = 0; i < audioStreamCount; i++) {
////        while (true) {
////            if (bNeedSend) {
////                //send NULL frame(flush frame) to encoder
////                ret = avcodec_send_frame(m_pAudioEncCtx[i], nullptr);
////                if (ret == AVERROR(EAGAIN)) {
////                    bNeedSend = true;   //frame need to be re-sent
////                } else if (ret == 0) {
////                    bNeedSend = false;    //send the frame successfully
////                } else if (ret < 0) {
////                    TELogcat::LogFFMpegError(TAG, ret, "_flushAudio avcodec_send_packet() failed");
////                    return COMBINE_I64ERROR(ret);
////                }
////            }
////
////            av_init_packet(m_pEncAudioPkt);
////            ret = avcodec_receive_packet(m_pAudioEncCtx[i], m_pEncAudioPkt);
////
////            if (ret ==
////                AVERROR(EAGAIN)) {//output is not available in the current state - user must try to send input
////                bNeedSend = true;
////                continue;
////            } else if (ret == AVERROR_EOF) {
////                VELOGI("TEFFMpegFileWriter _flushAudio, AVERROR_EOF");
////                break;
////            } else if (ret < 0) {
////                TELogcat::LogFFMpegError(TAG, ret, "_flushAudio avcodec_receive_packet() failed");
////                return COMBINE_I64ERROR(ret);
////            }
////
////#if defined(__IOS__)
////            if (m_pEncAudioPkt->duration != 0) {
////#endif
////
////            m_pEncAudioPkt->stream_index = m_pAudioStreams[i]->index;
////            m_pEncAudioPkt->pts = av_rescale_q(m_pEncAudioPkt->pts, m_pAudioEncCtx[i]->time_base,
////                                               m_pAudioStreams[i]->time_base);
////            m_pEncAudioPkt->dts = av_rescale_q(m_pEncAudioPkt->dts, m_pAudioEncCtx[i]->time_base,
////                                               m_pAudioStreams[i]->time_base);
////            m_pEncAudioPkt->duration = 0;
////
////            ret = writeAVPacketSafe(m_pEncAudioPkt, AVMEDIA_TYPE_AUDIO);
////            //av_packet_unref(m_pEncAudioPkt);
////
////            if (ret < 0) {
////                TELogcat::LogFFMpegError(
////                        TAG, ret, "_flushAudio av_interleaved_write_frame() failed, frame size: %d, pts: %" PRId64,
////                        m_pEncAudioPkt->size, m_pEncAudioPkt->pts);
////                break;
////            }
////            ++m_iWriteAudioFrameCount;
////#if defined(__IOS__)
////            } // endof  #if defined(__IOS__) if (m_pEncAudioPkt->duration != 0) {
////#endif
////
////        }
////        ret = writeAVPacketSafe(nullptr, AVMEDIA_TYPE_AUDIO);
////        if (ret < 0) {
////            TELogcat::LogFFMpegError(TAG, ret, "_flushAudio writeAVPacketSafe null audio failed");
////            return COMBINE_I64ERROR(ret);
////        } else {
////            VELOGI("TEFFMpegFileWriter _flushAudio writeAVPacketSafe null audio success");
////        }
////    }
////
////    return TER_OK;
////}
////
////
////list<ETEPixelFormat> TEFFMpegFileWriter::querySupportedHostInputVideoPixelFormat(char const* name) {
////    //todo::videoEncoder调用者传进来
////    list<ETEPixelFormat> listSupportedHostInputVideoPixelFormat;
////
////    // Find video codec
////    const AVCodecID codecId = videoCodecId();
////    AVCodec *videoEncoder;
////
////#if defined(__IOS__)
////    videoEncoder = avcodec_find_encoder(codecId);
////#else
////#if defined(__ANDROID__)
////    if (m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER)
////        videoEncoder = avcodec_find_encoder_by_name(ANDROID_HW_ENCODER_NAME);
////    else
////#endif
////    {
////        if (m_iFlags & TE_AV_FILE_WRITER_FLAG_GIF) {
////            videoEncoder = avcodec_find_encoder_by_name("gif");
////        } else {
////            if (TERuntimeConfig::s_bEnableByte264) {
////                videoEncoder = TEGetFFmpegVideoEncoder(codecId);
////            } else {
////#ifdef ENABLE_LIBX264
////                videoEncoder = avcodec_find_encoder_by_name("libx264");   // libopenh264
////                // Try ies x264
////                if (videoEncoder == nullptr) {
////                    videoEncoder = avcodec_find_encoder_by_name(IES_LIBX264_ENCODER_NAME);
////                }
////#elif ENABLE_LIBBYTE264
////                videoEncoder = avcodec_find_encoder_by_name(IES_LIBBYTE264_ENCODER_NAME);
////#else
////                videoEncoder = avcodec_find_encoder_by_name(ANDROID_HW_ENCODER_NAME);
////#endif
////            }
////        }
////    }
////#endif
////
////#if defined(_WIN32)
////    if (name) {
////        videoEncoder = avcodec_find_encoder_by_name(name);
////    }
////    if (!videoEncoder) {
////        videoEncoder = avcodec_find_encoder(codecId);
////    }
////#endif
////
////    if (m_bAlphaMovEncoding == true) {
////        videoEncoder = avcodec_find_encoder(AV_CODEC_ID_QTRLE);
////    }
////
////    if (!videoEncoder) {
////        VELOGE("Couldn't find video encoder for codec id=%d!", codecId);
////        return listSupportedHostInputVideoPixelFormat;
////    }
////
////    if (!videoEncoder->pix_fmts)
////        return listSupportedHostInputVideoPixelFormat;
////
////    for (int i = 0;; i++) {
////        const AVPixelFormat avPixFmt = videoEncoder->pix_fmts[i];
////        if (avPixFmt < 0)
////            break;
////
////        const ETEPixelFormat pixFmt = AVPixelFormatToTEPixelFormat(avPixFmt);
////        if (pixFmt != TEPixFmt_None)
////            listSupportedHostInputVideoPixelFormat.push_back(pixFmt);
////        if(pixFmt == TEPixFmt_QSV)
////            listSupportedHostInputVideoPixelFormat.push_back(TEPixFmt_QSV_D3D11);
////    }
////
////    return listSupportedHostInputVideoPixelFormat;
////}
////
////AVCodecID TEFFMpegFileWriter::videoCodecId() const {
////    {
////#if defined(_WIN32)
////        return AV_CODEC_ID_H264;
////#elif defined(__ANDROID__) || defined(__IOS__)
////        return AV_CODEC_ID_H264;
////#elif defined(__linux__)
////        return AV_CODEC_ID_H264;
////#else
////        return AV_CODEC_ID_MPEG4;
////#endif
////    }
////}
////
////list<ETEAudioSampleFormat> TEFFMpegFileWriter::querySupportedInputAudioSampleFormat() {
////    list<ETEAudioSampleFormat> listSupportedInputAudioSampleFormat;
////    AVCodec *audioEncoder = nullptr;
////
////#if defined(__ANDROID__)
////    if (m_audioFlags & TE_AV_FILE_AUDIO_FLAG_HW_ENCODER) {
////        audioEncoder = avcodec_find_encoder_by_name(VE_AAC_HW_ENCODER_NAME);
////    } else if (m_audioFlags & TE_AV_FILE_AUDIO_FLAG_AAC_ENCODER) {
////        audioEncoder = avcodec_find_encoder_by_name(IES_FFMPEG_AAC_ENCODER_NAME);
////    } else {
////        audioEncoder = avcodec_find_encoder_by_name("libfdk_aac");
////        if (audioEncoder == nullptr) {
////            audioEncoder = avcodec_find_encoder_by_name(IES_LIBFDKAAC_ENCODER_NAME);
////        }
////    }
////#elif defined(__IOS__)
////    audioEncoder = avcodec_find_encoder_by_name("aac_at");
////#else
////    audioEncoder = avcodec_find_encoder_by_name("libfdk_aac");
////    if (audioEncoder == nullptr) {
////        audioEncoder = avcodec_find_encoder_by_name(IES_LIBFDKAAC_ENCODER_NAME);
////    }
////#endif
////    if (!audioEncoder) {
////        VELOGE("Couldn't find audio encoder for aac");
////        return listSupportedInputAudioSampleFormat;
////    }
////
////    if (!audioEncoder->sample_fmts)
////        return listSupportedInputAudioSampleFormat;
////
////    int i = 0;
////    for (;; i++) {
////        const AVSampleFormat avSampleFmt = audioEncoder->sample_fmts[i];
////        if (avSampleFmt < 0)
////            break;
////
////        const ETEAudioSampleFormat sampleFmt = TEAVSampleFormatToTESampleFormat(avSampleFmt);
////        if (sampleFmt != TEAudSmpFmt_None)
////            listSupportedInputAudioSampleFormat.push_back(sampleFmt);
////    }
////
////    return listSupportedInputAudioSampleFormat;
////
////}
////
////AVCodecID TEFFMpegFileWriter::audioCodecId() const {
////    return AV_CODEC_ID_AAC;
////}
////
////int64_t TEFFMpegFileWriter::getVideoAssumeDuration() {
////    return m_iAssumedVPktDuration;
////}
////
////int64_t TEFFMpegFileWriter::addAudioStream(string sRemuxPath) {
////
////    int ret;
////    int64_t ret64 = 0L;
////    AVStream *audioStream = nullptr;
////    int ret_avformat = -1;
////    m_pInputFormatCtx = new TEAVFormatContext();
////    ret_avformat = te_avformat_open_input_custom(&m_pInputFormatCtx, sRemuxPath.c_str(), NULL, NULL);
////    if ((ret = ret_avformat) < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "Open remux file for audio failed");
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,TE_AVFORMAT_OPEN_INPUT_CUSTOM_FAILED);
////        goto end;
////    }
////
////    if ((ret = avformat_find_stream_info(m_pInputFormatCtx->formatContext, 0)) < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "Failed to retrieve input stream information");
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,AVFORMAT_FIND_STREAM_INFO_FAILED);
////        goto end;
////    }
////
////    ret = av_find_best_stream(m_pInputFormatCtx->formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
////    if (ret < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "Find stream for audio failed");
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,AV_FIND_BEST_STREAM_FAILED);
////        goto end;
////    }
////    m_nAudioStreamIndex = ret;
////
////    // Create audio stream
////    audioStream = avformat_new_stream(m_pOutputFormatCtx, nullptr);
////    if (!audioStream) {
////        ret = AVERROR(ENOMEM);
////        TELogcat::LogFFMpegError(TAG, ret, "avformat_new_stream failed");
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,AVFORMAT_NEW_STREAM_FAILED);
////        goto end;
////    }
////    // Stream id
////    audioStream->id = m_pOutputFormatCtx->nb_streams - 1;
////
////    m_pAudioStreams.push_back(audioStream);
////
////    ret = avcodec_parameters_copy(audioStream->codecpar,
////                                  m_pInputFormatCtx->formatContext->streams[m_nAudioStreamIndex]->codecpar);
////
////#if defined(__IOS__)
////    av_dict_set(&audioStream->metadata, "creation_time", m_creationDate.c_str(), 0);
////#endif
////    audioStream->codecpar->codec_tag = 0;
////    // if it is not set here, it depends avformat_write_header function
////    audioStream->time_base = m_pInputFormatCtx->formatContext->streams[m_nAudioStreamIndex]->time_base;
////    VELOGD("TEFFMpegFileWriter addAudioStream, sRemuxPath: %s", sRemuxPath.c_str());
////
////    if (ret < 0) {
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,AVCODEC_PARAMETERS_COPY_FAILED);
////        TELogcat::LogFFMpegError(TAG, ret, "avcodec_parameters_copy failed");
////        goto end;
////    }
////
////    end:
////    if (ret < 0) {
////        te_avformat_close_input_custom(&m_pInputFormatCtx);
////        return ret64;
////    }
////    addAudioReportCommonInfo(audioStream);
////    return TER_OK;
////}
////
////
////int64_t TEFFMpegFileWriter::addVideoStream(string sRemuxPath) {
////
////    int ret;
////    int64_t ret64 = 0L;
////    AVStream *videoStream = nullptr;
////    int ret_avformat = -1;
////    m_pInputFormatCtxV = new TEAVFormatContext();
////    ret_avformat = te_avformat_open_input_custom(&m_pInputFormatCtxV, sRemuxPath.c_str(), NULL, NULL);
////
////    if ((ret = ret_avformat) < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "Open remux file for video failed");
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,TE_AVFORMAT_OPEN_INPUT_CUSTOM_FAILED);
////
////        TELogcat::LogFFMpegError(TAG, ret, "ret = %d", ret);
////
////        if (!TEUtils::isFileExist(sRemuxPath.c_str())) {
////            TELogcat::LogFFMpegError(TAG, ret, "sRemuxPath: %s is not exist", sRemuxPath.c_str());
////        }
////
////        goto end;
////    }
////
////    if ((ret = avformat_find_stream_info(m_pInputFormatCtxV->formatContext, 0)) < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "Failed to retrieve input stream information");
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,AVFORMAT_FIND_STREAM_INFO_FAILED);
////        goto end;
////    }
////
////    ret = av_find_best_stream(m_pInputFormatCtxV->formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
////    if (ret < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "Find stream for audio failed");
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,AV_FIND_BEST_STREAM_FAILED);
////        goto end;
////    }
////    m_nVideoStreamIndex = ret;
////
////    // Create audio stream
////    videoStream = avformat_new_stream(m_pOutputFormatCtx, NULL);
////    if (!videoStream) {
////        ret =  AVERROR(ENOMEM);
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,AVFORMAT_NEW_STREAM_FAILED);
////        TELogcat::LogFFMpegError(TAG, ret, "avformat_new_stream failed");
////        goto end;
////    }
////
////    av_dict_copy(&videoStream->metadata,
////                 m_pInputFormatCtxV->formatContext->streams[m_nVideoStreamIndex]->metadata,
////                 0);
////
////#if defined(__IOS__)
////    av_dict_set(&videoStream->metadata, "creation_time", m_creationDate.c_str(), 0);
////#endif
////
////    // Stream id
////    videoStream->id = m_pOutputFormatCtx->nb_streams - 1;
////    m_pVideoStream = videoStream;
////    // remux video stream extradata is ready
////    m_videoEncodeExtradataReady = true;
////
////    ret = avcodec_parameters_copy(m_pVideoStream->codecpar,
////                                  m_pInputFormatCtxV->formatContext->streams[m_nVideoStreamIndex]->codecpar);
////    m_pVideoStream->sample_aspect_ratio = m_pInputFormatCtxV->formatContext->streams[m_nVideoStreamIndex]->sample_aspect_ratio;
////    // apple only supports hvc1 type of HEVC video, since we are able to demux other codec_tag types, we cannot just pass-through codec_tag, instead we need to set it strictly to hvc1.
////    if (videoStream->codecpar->codec_id == AV_CODEC_ID_HEVC) {
////        // ios hevc codec_tag must be hvc1, ffmpeg defaults to hev1, so we manually set hvc1 here
////        videoStream->codecpar->codec_tag = MKTAG('h', 'v', 'c', '1');
////    } else {
////        videoStream->codecpar->codec_tag = 0;
////    }
////    // if it is not set here, it depends avformat_write_header function
////    m_pVideoStream->time_base = m_pInputFormatCtxV->formatContext->streams[m_nVideoStreamIndex]->time_base;
////    VELOGD("TEFFMpegFileWriter addVideoStream, sRemuxPath: %s", sRemuxPath.c_str());
////
////    if (ret < 0) {
////        ret64 = COMBINE_I64ERROR_WITH_EXT(ret,AVCODEC_PARAMETERS_COPY_FAILED);
////        TELogcat::LogFFMpegError(TAG, ret, "avcodec_parameters_copy failed");
////        goto end;
////    }
////
////    end:
////    if (ret < 0) {
////        te_avformat_close_input_custom(&m_pInputFormatCtxV);
////        return ret64;
////    }
////    if (reportPtr != nullptr) {
////        reportPtr->addCommonInfo("encode_type", "remux");
////    }
////    addVideoReportCommonInfo(m_pVideoStream);
////    return TER_OK;
////}
////
////int64_t TEFFMpegFileWriter::addAudioStream(const STEAudioResolution &in_audioRes) {
////    if (!isValid())
////        return TER_INVALID_CONTEXT;
////    if (m_bStarted)
////        return TER_INVALID_STATE;
////    if (m_iFlags&TE_AV_FILE_WRITER_FLAG_GIF)
////        return COMBINE_I64ERROR_WITH_EXT(TER_INVALID_STATE,TE_AV_FILE_WRITER_FLAG_GIF);
////    if(m_iFlags&TE_AV_FILE_WRITER_FLAG_SINGLE_VIDEO)
////        return COMBINE_I64ERROR_WITH_EXT(TER_INVALID_STATE,TE_AV_FILE_WRITER_FLAG_SINGLE_VIDEO);
////
////    // Find audio encoder
////    const AVCodecID codecId = audioCodecId();
////    AVCodec *audioEncoder = nullptr;
////
////#if defined(__ANDROID__)
////    if (m_audioFlags & TE_AV_FILE_AUDIO_FLAG_HW_ENCODER) {
////        audioEncoder = avcodec_find_encoder_by_name(VE_AAC_HW_ENCODER_NAME);
////    } else if (m_audioFlags & TE_AV_FILE_AUDIO_FLAG_AAC_ENCODER) {
////        audioEncoder = avcodec_find_encoder_by_name(IES_FFMPEG_AAC_ENCODER_NAME);
////    } else {
////        audioEncoder = avcodec_find_encoder_by_name("libfdk_aac");
////        if (audioEncoder == nullptr) {
////            audioEncoder = avcodec_find_encoder_by_name(IES_LIBFDKAAC_ENCODER_NAME);
////        }
////    }
////#elif defined(__IOS__)
////    audioEncoder = avcodec_find_encoder_by_name("aac_at");
////#else
////    audioEncoder = avcodec_find_encoder_by_name("libfdk_aac");
////    if (audioEncoder == nullptr) {
////        audioEncoder = avcodec_find_encoder_by_name(IES_LIBFDKAAC_ENCODER_NAME);
////    }
////#endif
////
////    if (!audioEncoder) {
////        VELOGE("Couldn't find audio encoder for codec id=%d!", codecId);
////        addReportSimpleErrorInfo("addAudioStream", "find audio encoder for codec");
////        return AVERROR_ENCODER_NOT_FOUND;
////    }
////
////    // Create audio stream
////    AVStream *audioStream = avformat_new_stream(m_pOutputFormatCtx, audioEncoder);
////    if (!audioStream) {
////        VELOGE("Failed to add audio stream!");
////        addReportSimpleErrorInfo("addAudioStream", "Failed to add audio stream");
////        return AVERROR(ENOMEM);
////    }
////    // Stream id
////    audioStream->id = m_pOutputFormatCtx->nb_streams - 1;
////
////    m_pAudioStreams.push_back(audioStream);
////
////    //
////    // Open audio encoder
////    //
////    //allocate a codec ctx, because the codec ctx in avstream is
////    AVCodecContext *audioEncoderCtx = avcodec_alloc_context3(audioEncoder);
////    audioEncoderCtx->codec_id = audioEncoder->id;
////
////    // Set sample format
////    audioEncoderCtx->sample_fmt = TESampleFormatToAVSampleFormat(in_audioRes.eSampleFormat);
////
////    // Average bit rate
////    audioEncoderCtx->bit_rate = in_audioRes.nBitRate;
////
////    // Sample rate
////    audioEncoderCtx->sample_rate = in_audioRes.nSampleRate;
////
////    // Time base
////    audioEncoderCtx->time_base.num = 1;
////    audioEncoderCtx->time_base.den = audioEncoderCtx->sample_rate;
////
////    audioStream->time_base.num = 1;
////    audioStream->time_base.den = audioEncoderCtx->sample_rate;
////
////    //  suyang todo later:  > WARNING codec timebase is very high. If duration is too long,
////    //    file may not be playable by quicktime. Specify a shorter timebase
////    //    or choose different container.
////    //
////    // Determine audio channel layout
////    //
////    uint64_t channelLayout = 0;
////    if (audioEncoder->channel_layouts) {
////        int layoutIndex = 0;
////        for (;;) {
////            const uint64_t layout = audioEncoder->channel_layouts[layoutIndex];
////            if (!layout)
////                break;
////
////            const int channelCount = av_get_channel_layout_nb_channels(layout);
////            if ((unsigned int) channelCount == in_audioRes.nChannelCount) {
////                channelLayout = layout;
////                break;
////            }
////
////            ++layoutIndex;
////        }
////    }
////    if (!channelLayout)
////        channelLayout = static_cast<uint64_t>(av_get_default_channel_layout(
////                in_audioRes.nChannelCount));
////
////    audioEncoderCtx->channel_layout = channelLayout;
////
////    // Channel count
////    audioEncoderCtx->channels = av_get_channel_layout_nb_channels(audioEncoderCtx->channel_layout);
////
////    // Some formats want stream headers to be separate
////    if (m_pOutputFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
////        audioEncoderCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
////
////#ifndef __ANDROID__
////    if (audioEncoder->capabilities & CODEC_CAP_AUTO_THREADS)
////        audioEncoderCtx->thread_count = 0;
////    else
////#endif
////        //audioEncoderCtx->thread_count = NvGetConfiguredProcessorCount();
////        audioEncoderCtx->thread_count = av_cpu_count();
////#if defined(TARGET_OS_LINUX)
////    audioEncoderCtx->thread_count = 4;
////#endif
////
////    if (audioEncoder->capabilities & CODEC_CAP_FRAME_THREADS) {
////        audioEncoderCtx->thread_type = FF_THREAD_FRAME;
////    } else if (audioEncoder->capabilities & CODEC_CAP_SLICE_THREADS) {
////        audioEncoderCtx->thread_type = FF_THREAD_SLICE;
////    } else {
////        // NOTE: some encoder (such as libx264) support multi-threaded encoding,
////        // but they don't report it in CODEC_CAP_FRAME_THREADS/CODEC_CAP_SLICE_THREADS flags
////        // in that case, we prefer frame thread model
////        audioEncoderCtx->thread_type = FF_THREAD_FRAME;
////    }
////
////#if !defined(_WIN32) && !defined(_WIN64)
////    if (codecId == AV_CODEC_ID_AAC) {
////        // NOTE: the builtin AAC encoder in ffmpeg is experimental
////        // We have to set FF_COMPLIANCE_EXPERIMENTAL flag to let it work
////        audioEncoderCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
////    }
////#endif
////
////#if defined(__IOS__)
////    av_dict_set(&audioStream->metadata, "creation_time", m_creationDate.c_str(), 0);
////#endif
////    TEFFmpegRegister::enableFFMpegLogCat(true);
////
////    // Open encoder context
////    AVDictionary *opt = nullptr;
////#if defined(__ANDROID__)
////    if (!strcmp(VE_AAC_HW_ENCODER_NAME, audioEncoder->name)) {
////        av_dict_set(&opt, "aac_output_mode", AAC_OUTPUT_MODE, 0);
////    }
////#endif
////    int ret = avcodec_open2(audioEncoderCtx, audioEncoder, opt ? &opt : 0);
////    av_dict_free(&opt);
////    if (ret < 0) {
////        int detailErrorCode = TER_AUDIO_SW_INIT_FAILURE;
////#if defined(__ANDROID__)
////        if (strcmp(VE_AAC_HW_ENCODER_NAME, audioEncoder->name)) {
////            detailErrorCode = TER_AUDIO_SW_INIT_FAILURE;
////        } else {
////            detailErrorCode = TER_AUDIO_HW_INIT_FAILURE;
////        }
////#endif
////        TELogcat::LogFFMpegError(TAG, ret, "TEFFMpegFileWriter avcodec_open2() audio failed, name: %s", audioEncoder->name);
////        cleanup();
////        return COMBINE_I64ERROR_WITH_EXT(ret, detailErrorCode);
////    }
////
////    ret = avcodec_parameters_from_context(audioStream->codecpar, audioEncoderCtx);
////    if (ret < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "avcodec_parameters_from_context() failed! error");
////        cleanup();
////        addReportSimpleErrorInfo("addAudioStream", "avcodec_parameters_from_context() failed! error");
////        return COMBINE_I64ERROR_WITH_EXT(ret,AVCODEC_PARAMETERS_FROM_CONTEXT_FAILED);
////    }
////
////    // determine whether to include audio extradata
////#if defined(__ANDROID__)
////    m_audioHw = !strcmp(VE_AAC_HW_ENCODER_NAME, audioEncoder->name);
////    if (m_audioHw && strcmp(AAC_OUTPUT_MODE, "2")) {
////        if (audioStream->codecpar->extradata && audioStream->codecpar->extradata_size > 0) {
////            TEPerfStats::perfLong(MONITOR_COMPOSITION, TETRACKER_KEY_COMPOSITION_AUDIO_HW_ENCODER_EXTRADATA, 0);
////        } else {
////            TEPerfStats::perfLong(MONITOR_COMPOSITION, TETRACKER_KEY_COMPOSITION_AUDIO_HW_ENCODER_EXTRADATA, 1);
////        }
////    }
////#else
////    m_audioHw = false;
////#endif
////    VELOGI("TEFFMpegFileWriter avcodec_open2, extradata_size: %d, name: %s", audioStream->codecpar->extradata_size, audioEncoder->name);
////
////#ifdef DEBUG
////    m_pMapUserMetaData.emplace("audio", audioEncoder->name);
////#endif
////    addAudioReportCommonInfo(audioStream);
////    m_pAudioEncCtx.push_back(audioEncoderCtx);
////    return TER_OK;
////}
////
////
////int64_t
////TEFFMpegFileWriter::addVideoStream(TEBundle &encodeParams) {
////    if (!isValid()) {
////        addReportSimpleErrorInfo("addVideoStream TEBundle", "invalid context");
////        return TER_INVALID_CONTEXT;
////    }
////
////    if (m_bStarted) {
////        addReportSimpleErrorInfo("addVideoStream TEBundle", "not start");
////        return TER_INVALID_STATE;
////    }
////
////    if (m_pVideoStream) {
////        addReportSimpleErrorInfo("addVideoStream TEBundle", " has m_pVideoStream already ");
////        return TER_FAIL; // You can't add more than one video stream!
////    }
////
////    const char *key = VIDEO_FILE_WRITING_MODE;
////    if (encodeParams.containsKey(key)) {
////        m_writeMode = static_cast<VIDEO_WRITING_MODE >(encodeParams.getInt(key));
////    }
////
////    if (VIDEO_WRITING_MUX_ONLY == m_writeMode) {
////        return addVideoStreamForMux(encodeParams);
////    }
////    STEVideoResolution in_videoRes = *static_cast<STEVideoResolution *>(encodeParams.getHandle(VIDEO_ENCODE_RESOLUTION));
////    ETEPixelFormat in_inputImagePixelFormat = static_cast<ETEPixelFormat>(encodeParams.getInt(VIDEO_ENCODE_FRAME_TYPE));
////    ENCODE_BITRATE_MODE biterateMode = static_cast<ENCODE_BITRATE_MODE >(encodeParams.getInt(VIDEO_ENCODE_BITERATE_MODE));
////    int bps = static_cast<int>(encodeParams.getInt(VIDEO_ENCODE_BITRATE));
////    int qp = static_cast<int>(encodeParams.getInt(VIDEO_ENCODE_QP));
////    STERational in_avgFps = *static_cast<STERational *>(encodeParams.getHandle(VIDEO_ENCODE_AVERAGE_FPS));
////
////    pixelformat = (int)in_inputImagePixelFormat;
////
////#if !defined(__IOS__)
////#if defined(_WIN32)
////    auto encoder_name = encodeParams.getString(COMPILE_ENCODER_NAME);
////    list<ETEPixelFormat> inputFormatList(querySupportedHostInputVideoPixelFormat(encoder_name ? encoder_name->data() : nullptr));
////#else
////    list<ETEPixelFormat> inputFormatList(querySupportedHostInputVideoPixelFormat());
////#endif
////    auto iter = find(inputFormatList.begin(), inputFormatList.end(), in_inputImagePixelFormat);
////    if (iter == inputFormatList.end()) {
////        VELOGE("Invalid input image pixel format %d!", in_inputImagePixelFormat);
////        addReportSimpleErrorInfo("addVideoStream TEBundle", " Invalid input image pixel format ");
////        return TER_INVALID_FRAME_FORMAT;
////    }
////#endif
////
////    AVCodecID codecId;
////    AVCodec *videoEncoder;
////    string codecName = "";
////
////#if defined(__IOS__)
////    codecName = "h264_videotoolbox";
////    videoEncoder = avcodec_find_encoder_by_name("h264_videotoolbox");
////#else
////#if defined(__ANDROID__)
////    if (m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER) {
////        if (m_iFlags & TE_AV_FILE_WRITER_FLAG_MPEG4) {
////            codecName = ANDROID_HW_ENCODER_NAME_MPEG4;
////        } else {
////            codecName = ANDROID_HW_ENCODER_NAME;
////        }
////    } else
////#endif
////    {
////        if (m_iFlags & TE_AV_FILE_WRITER_FLAG_GIF)
////            codecName = "gif";
////        else {
////            if (TERuntimeConfig::s_bEnableByte264) {
////#ifdef ENABLE_LIBBYTE264
////                codecName = IES_LIBBYTE264_ENCODER_NAME;
////                VELOGI("use video Encoder: libbyte264");
////#endif
////            } else {
////#ifdef ENABLE_LIBX264
////                codecName = "libx264";   // libopenh264
////#elif ENABLE_LIBBYTE264
////                codecName = IES_LIBBYTE264_ENCODER_NAME;
////#elif defined(_WIN32)
////                codecName = *encodeParams.getString(COMPILE_ENCODER_NAME);
////                assert(codecName != "");
////#else
////                codecName = ANDROID_HW_ENCODER_NAME;   // 没有启用LIBX264，就用硬编
////#endif
////            }
////        }
////    }
////#endif // endof defined(__IOS__)
////
////
////    videoEncoder = avcodec_find_encoder_by_name(codecName.c_str());
////
////    if (m_bAlphaMovEncoding) {
////        videoEncoder = avcodec_find_encoder(AV_CODEC_ID_QTRLE);
////    }
////
////    // Try IES x264
////    if (videoEncoder == nullptr) {
////        videoEncoder = TEGetFFmpegVideoEncoder(AV_CODEC_ID_H264);
////    }
////
////    if (!videoEncoder) {
////        VELOGE("Couldn't find video encoder for codec name=%s!", codecName.c_str());
////        addReportSimpleErrorInfo("addVideoStream TEBundle codec name", codecName.c_str());
////        return AVERROR_ENCODER_NOT_FOUND;
////    }
////
////#if defined(__ANDROID__)
////    if (strcmp(ANDROID_HW_ENCODER_NAME, videoEncoder->name)) {
////        m_videoEncodeExtradataReady = true;
////        VELOGI("TEFFMpegFileWriter addVideoStream, video soft encode: %s", videoEncoder->name);
////    } else if (0 == strcmp(ANDROID_HW_ENCODER_NAME_MPEG4, videoEncoder->name)) {
////        m_videoEncodeExtradataReady = false;
////        VELOGI("TEFFMpegFileWriter addVideoStream, mpeg4 video soft encode: %s", videoEncoder->name);
////    } else {
////        m_videoEncodeExtradataReady = false;
////        VELOGI("TEFFMpegFileWriter addVideoStream, video hw encode: %s", videoEncoder->name);
////    }
////#else
////    m_videoEncodeExtradataReady = true;
////#endif
////
////    codecId = videoEncoder->id;
////
////    // Create video stream
////    m_pVideoStream = avformat_new_stream(m_pOutputFormatCtx, videoEncoder);
////    if (!m_pVideoStream) {
////        VELOGE("Failed to add video stream!");
////        addReportSimpleErrorInfo("addVideoStream TEBundle new stream failed", codecName.c_str());
////        return AVERROR(ENOMEM);
////    }
////
////    // Stream id
////    m_pVideoStream->id = m_pOutputFormatCtx->nb_streams - 1;
////
////    // Pixel aspect ratio
////    if (!in_videoRes.sImagePAR.num || !in_videoRes.sImagePAR.den) {
////        VELOGE("Invalid pixel aspect ratio!");
////        addReportSimpleErrorInfo("addVideoStream TEBundle", "Invalid pixel aspect ratio");
////        cleanup();
////        return TER_UNSUPPORTED;
////    }
////
////    //
////    // Open video encoder context
////    //
////    AVDictionary *opt = nullptr;
////
////    m_pVideoEncCtx = avcodec_alloc_context3(videoEncoder);
////    m_pVideoEncCtx->codec_id = videoEncoder->id;
////    int framerate;
////    if(in_avgFps.num > 0) {
////        // Set timebase (1/framerate) used by encoder
////        framerate = in_avgFps.num / in_avgFps.den;
////        if (framerate < 5) {
////            framerate = 7;
////        }
////
////        m_pVideoEncCtx->framerate = {framerate, 1};
////        m_pVideoStream->avg_frame_rate = {framerate, 1};
////    }else {
////        framerate = DEFAULT_FPS;
////        m_pVideoEncCtx->framerate = {framerate, 1};
////        m_pVideoStream->avg_frame_rate = {framerate, 1};
////    }
////    m_pVideoEncCtx->time_base = {1, TE_TIME_BASE};
////    m_pVideoStream->time_base = m_pVideoEncCtx->time_base;
////
////
////#if !defined(__IOS__)
////    if (TERuntimeConfig::s_bColorSpaceRepair) { // android use FFMpeg symbols directly, which conflict with avfoundation. so on ios, we use vtconfig data to specify color primaries/ transfer func/ ycrcb matrix
////        AVColorSpace colorSpace = (AVColorSpace)encodeParams.getInt(COMPILE_COLOR_SPACE);
////        if (AVCOL_SPC_BT2020_NCL == colorSpace) {
////            m_pVideoEncCtx->color_primaries = AVCOL_PRI_BT2020;
////            m_pVideoEncCtx->color_trc = AVCOL_TRC_BT2020_10;
////            m_pVideoEncCtx->colorspace = AVCOL_SPC_BT2020_NCL;
////        } else if (AVCOL_SPC_BT709 == colorSpace) {
////            m_pVideoEncCtx->color_primaries = AVCOL_PRI_BT709;
////            m_pVideoEncCtx->color_trc = AVCOL_TRC_BT709; // used to determine transfer func
////            m_pVideoEncCtx->colorspace = AVCOL_SPC_BT709; // used to determine ycrcbmatrix
////        } else if (AVCOL_SPC_SMPTE170M == colorSpace) {
////            m_pVideoEncCtx->color_primaries = AVCOL_PRI_SMPTE170M;
////            m_pVideoEncCtx->color_trc = AVCOL_TRC_SMPTE170M;
////            m_pVideoEncCtx->colorspace = AVCOL_SPC_SMPTE170M;
////        } else {
////            m_pVideoEncCtx->color_primaries = AVCOL_PRI_UNSPECIFIED;
////            m_pVideoEncCtx->color_trc = AVCOL_TRC_UNSPECIFIED;
////            m_pVideoEncCtx->colorspace = AVCOL_SPC_UNSPECIFIED;
////        }
////
////        m_pVideoEncCtx->color_range = (AVColorRange)encodeParams.getInt(COMPILE_COLOR_RANGE);
////
////        if (TERuntimeConfig::s_bColorSpaceRepairFor2020) {
////            m_pVideoEncCtx->color_trc = (AVColorTransferCharacteristic)encodeParams.getInt(COMPILE_COLOR_TRC);
////            m_pVideoEncCtx->color_primaries = (AVColorPrimaries)encodeParams.getInt(COMPILE_COLOR_PRI);
////        }
////    }
////
////    // Setup profile and level
////    if (codecId == AV_CODEC_ID_H264) {
////        int profile = static_cast<int>(encodeParams.getInt(VIDEO_ENCODE_PROFILE));
////        VELOGD("AV_CODEC_ID_H264: profile %d", profile);
////        if( profile > 0 ) {
////            if (m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER) {
////                m_pVideoEncCtx->profile = profile;
////            } else {
////                switch(profile) {       //profile: 1-ENCODE_PROFILE_BASELINE; 2-ENCODE_PROFILE_MAIN; 3-ENCODE_PROFILE_HIGH
////                    case 1:
////                        m_pVideoEncCtx->profile = FF_PROFILE_H264_BASELINE;
////                        break;
////                    case 2:
////                        m_pVideoEncCtx->profile = FF_PROFILE_H264_MAIN;
////                        break;
////                    case 3:
////                        m_pVideoEncCtx->profile = FF_PROFILE_H264_HIGH;
////                        break;
////                    default:
////                        break;
////                }
////            }
////        }
////    }
////#endif
////
////    double highBitrateRatio = static_cast<double>(encodeParams.getDouble(VIDEO_ENCODE_HP_BITRATE_RATIO));
////    double transitionKeyFrameBitrateRatio = static_cast<double>(encodeParams.getDouble(VIDEO_ENCODE_TRANSITION_KEYFRAME_BITRATE_RATIO));
////
////    tagSTEEncoderParamsOpaque opaqueInfo {};
////    opaqueInfo.dHpBitrateRatio = highBitrateRatio;
////    opaqueInfo.dTransitionBitrateRatio = transitionKeyFrameBitrateRatio;
////    m_pVideoEncCtx->opaque = av_mallocz(sizeof(tagSTEEncoderParamsOpaque));
////    memcpy(m_pVideoEncCtx->opaque, &opaqueInfo, sizeof(tagSTEEncoderParamsOpaque));
////
////    // Image size
////    m_pVideoEncCtx->width = (int) in_videoRes.nImageWidth;
////    m_pVideoEncCtx->height = (int) in_videoRes.nImageHeight;
////
////#if defined(__IOS__)
////    m_pVideoEncCtx->pix_fmt = AV_PIX_FMT_VIDEOTOOLBOX;//(AVPixelFormat)TEPixelFormatToAVPixelFormat(in_inputImagePixelFormat);
////
////    m_pVideoEncCtx->has_b_frames = m_pVTConfigData.hasBFrame;
////    m_pVideoEncCtx->max_b_frames = m_pVTConfigData.hasBFrame ? 1 : 0; //ffmpeg vt 不会用到这个值，只关心是不是大于0
////#else // else defined(__IOS__)
////    // Pixel aspect ratio
////    m_pVideoEncCtx->sample_aspect_ratio = m_pVideoStream->sample_aspect_ratio;
////
////    // Set pixel format
////    m_pVideoEncCtx->pix_fmt = (AVPixelFormat)TEPixelFormatToAVPixelFormat(in_inputImagePixelFormat);
////#if defined(_WIN32)
////    if (in_inputImagePixelFormat == TEPixFmt_CUDA ||
////        in_inputImagePixelFormat == TEPixFmt_QSV ||
////        in_inputImagePixelFormat == TEPixFmt_QSV_D3D11 ||
////        in_inputImagePixelFormat == TEPixFmt_D3D11_Texture2D)
////    {
////        bool bIntel = codecName == "h264_qsv"s;
////        AVHWDeviceType dev_type = AV_HWDEVICE_TYPE_QSV;
////        if (in_inputImagePixelFormat == TEPixFmt_CUDA)
////            dev_type = AV_HWDEVICE_TYPE_CUDA;
////        else if (in_inputImagePixelFormat == TEPixFmt_D3D11_Texture2D)
////            dev_type = AV_HWDEVICE_TYPE_D3D11VA;
////        AVBufferRef *hw_device_ctx = NULL;
////        AVDictionary* opt = nullptr;
////        auto d3d11dev = encodeParams.getInt("TED3D11Dev");
////        auto device = TEGpuDeviceManager::instance().getGpuDevice();
////        if (in_inputImagePixelFormat == TEPixFmt_QSV ||
////            in_inputImagePixelFormat == TEPixFmt_QSV_D3D11) {
////            if (in_inputImagePixelFormat == TEPixFmt_QSV)
////            {
////                uint64_t mgr = (uint64_t)TED3D9DeviceManager::getInstance()->getDeviceManager();
////                uint64_t dev = (uint64_t)TED3D9DeviceManager::getInstance()->getDevice();
////                uint64_t d3d9 = (uint64_t)TED3D9DeviceManager::getInstance()->getD3D9();
////
////                std::string m = std::to_string(mgr);
////                std::string d = std::to_string(dev);
////                std::string t = std::to_string(d3d9);
////                av_dict_set(&opt, "TED3D9Mgr", m.c_str(), 0);
////                av_dict_set(&opt, "TED3D9Dev", d.c_str(), 0);
////                av_dict_set(&opt, "TED3D9", t.c_str(), 0);
////
////                m_ePixFmt = TEPixFmt_QSV;
////            }
////            else
////            {
////                av_dict_set(&opt, "TED3D11Dev", std::to_string(d3d11dev).data(), 0);
////                m_ePixFmt = TEPixFmt_QSV_D3D11;
////            }
////
////            device = "hw";
////        }
////        else if (dev_type == AV_HWDEVICE_TYPE_D3D11VA) {
////            if (d3d11dev) {
////                av_dict_set(&opt, "TED3D11Dev", std::to_string(d3d11dev).data(), 0);
////            }
////        }
////
////        auto err = av_hwdevice_ctx_create(&hw_device_ctx, dev_type, device.data(), opt, 0);
////        av_dict_free(&opt);
////        if (err < 0)
////        {
////            char err_buffer[AV_ERROR_MAX_STRING_SIZE] = { 0 };
////            VELOGE("Failed to create a CUDA device. Error code: %s", av_make_error_string(err_buffer, sizeof(err_buffer), err));
////            return err;
////        }
////
////        auto set_hwframe_ctx = [=](AVCodecContext* ctx, AVBufferRef* hw_device_ctx) -> int
////        {
////            AVBufferRef* hw_frames_ref;
////            AVHWFramesContext* frames_ctx = NULL;
////            int err = 0;
////
////            if (!(hw_frames_ref = av_hwframe_ctx_alloc(hw_device_ctx)))
////            {
////                VELOGE("Failed to create CUDA frame context.");
////                return -1;
////            }
////
////            frames_ctx = (AVHWFramesContext*)(hw_frames_ref->data);
////            frames_ctx->format = AV_PIX_FMT_CUDA;
////            frames_ctx->sw_format = AV_PIX_FMT_NV12;
////            frames_ctx->width = ctx->width;
////            frames_ctx->height = ctx->height;
////
////            if (dev_type == AV_HWDEVICE_TYPE_QSV) {
////                frames_ctx->format = AV_PIX_FMT_QSV;
////                frames_ctx->initial_pool_size = 10;
////            }
////            else if (dev_type == AV_HWDEVICE_TYPE_D3D11VA) {
////                frames_ctx->format = AV_PIX_FMT_D3D11;
////            }
////
////            if ((err = av_hwframe_ctx_init(hw_frames_ref)) < 0)
////            {
////                char err_buffer[AV_ERROR_MAX_STRING_SIZE] = { 0 };
////                VELOGE("Failed to initialize CUDA frame context. Error code: %s\n", av_make_error_string(err_buffer, sizeof(err_buffer), err));
////                av_buffer_unref(&hw_frames_ref);
////                return err;
////            }
////
////            ctx->hw_frames_ctx = av_buffer_ref(hw_frames_ref);
////            if (!ctx->hw_frames_ctx)
////            {
////                err = AVERROR(ENOMEM);
////            }
////
////            av_buffer_unref(&hw_frames_ref);
////            return err;
////        };
////
////        if ((err = set_hwframe_ctx(m_pVideoEncCtx, hw_device_ctx)) < 0)
////        {
////            VELOGE("Failed to init CUDA frame context!");
////            return err;
////        }
////
////        m_pEncVideoFrame->hw_frames_ctx = av_buffer_ref(m_pVideoEncCtx->hw_frames_ctx);
////    }
////#endif
////
////    //
////    // GOP size control
////    //
////    if (codecId == AV_CODEC_ID_H264) {
////        if (m_iFlags & TE_AV_FILE_WRITER_FLAG_OPTIMIZED_FOR_SHARING)
////            m_pVideoEncCtx->gop_size = 60;
////        else
////            m_pVideoEncCtx->gop_size = 30;
////    } else {
////        m_pVideoEncCtx->gop_size = 30;
////    }
////
////    if (m_bAllIFrameEncoding) {
////        m_pVideoEncCtx->gop_size = 0;
////    }
////
////    //user define gop size
////    int64_t tmp;
////    if (encodeParams.getInt(VIDEO_ENCODE_GOP_SIZE, tmp) && tmp >= 0) {
////        m_pVideoEncCtx->gop_size = (int) tmp;
////    }
////    m_pVideoEncCtx->ticks_per_frame = 2;
////#ifdef ENABLE_LIBBYTE264
////    if (TERuntimeConfig::s_bEnableByte264) {
////       m_pVideoEncCtx->ticks_per_frame = TE_TIME_BASE / 50;
////    }
////#endif
////
////    m_bIsEnableMoovFront = encodeParams.getBool(VIDEO_ENCODE_MOOV_FRONT);
//////    // B-frame count
//////    if (codecId != AV_CODEC_ID_MPEG4)
//////        m_pVideoEncCtx->max_b_frames = 2;
//////    else
//////        m_pVideoEncCtx->max_b_frames = 0; // For some unknown reason, We will get a teared video if we use mpeg4 advanced simple profile
////
////    if (!(m_iFlags & TE_AV_FILE_WRITER_FLAG_OPTIMIZED_FOR_SHARING)) {
////        m_pVideoEncCtx->flags |= CODEC_FLAG_CLOSED_GOP;
////        if (codecId != AV_CODEC_ID_H264) {
////            // NOTE: For some unknown reason, many encoder (at least except libx264)
////            // will fail to open if we set CODEC_FLAG_CLOSED_GOP flag,
////            // We need to set scenechange_threshold to a value at least 1000000000
////            // Please refer to http://ffmpeg.org/pipermail/libav-user/2012-June/002331.html
////            m_pVideoEncCtx->scenechange_threshold = 1000000000;
////        }
////    }
////#endif // end of defined(__IOS__)
////
////    //videoEncoderCtx->thread_count = NvGetConfiguredProcessorCount();
////    m_pVideoEncCtx->thread_count = av_cpu_count();
////#if defined(TARGET_OS_LINUX)
////    m_pVideoEncCtx->thread_count = 16;
////#endif
////    if (m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER) {
////        m_pVideoEncCtx->thread_count = 1;
////    }
////
////    if (videoEncoder->capabilities & CODEC_CAP_FRAME_THREADS) {
////        m_pVideoEncCtx->thread_type = FF_THREAD_FRAME;
////    } else if (videoEncoder->capabilities & CODEC_CAP_SLICE_THREADS) {
////        m_pVideoEncCtx->thread_type = FF_THREAD_SLICE;
////    } else {
////        // NOTE: some encoder (such as libx264) support multi-threaded encoding,
////        // but they don't report it in CODEC_CAP_FRAME_THREADS/CODEC_CAP_SLICE_THREADS flags
////        // in that case, we prefer frame thread model
////        m_pVideoEncCtx->thread_type = FF_THREAD_FRAME;
////    }
////
////    // Some formats want stream headers to be separate
////    if (m_pOutputFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
////        m_pVideoEncCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
////
////    //Setting video rotation
////    string strRotation;
////    switch (in_videoRes.eImageRotation) {
////        case TEVideoRotation_0:
////            strRotation = "0";
////            break;
////        case TEVideoRotation_90:
////            strRotation = "90";
////            break;
////        case TEVideoRotation_180:
////            strRotation = "180";
////            break;
////        case TEVideoRotation_270:
////            strRotation = "270";
////            break;
////        default:
////            strRotation = "0";
////            break;
////    }
////    int ret = av_dict_set(&m_pVideoStream->metadata, "rotate", strRotation.c_str(), 0);
////#if defined(__IOS__)
////    av_dict_set(&m_pVideoStream->metadata, "creation_time", m_creationDate.c_str(), 0);
////#endif
////
////    if (ret) {
////        VELOGE("YXFFMpegFileWriter::addVideoStream(): set rotation angle failed");
////    }
////#if defined(__ANDROID__)
////    if (codecName == ANDROID_HW_ENCODER_NAME || codecName == ANDROID_HW_ENCODER_NAME_MPEG4) {
////        if (codecName == ANDROID_HW_ENCODER_NAME_MPEG4) {
////            av_dict_set(&opt, "codecType", "mpeg4", 0);
////        } else {
////            av_dict_set(&opt, "codecType", "h264", 0);
////        }
////        av_dict_set_int(&opt, "sharedProcessQueue", reinterpret_cast<int64_t>(m_glProcessQueue), 0);
////        if (TERuntimeConfig::s_bEnableReEncodeOpt) {
////            av_dict_set_int(&opt, "mediaCodecDecInstance", reinterpret_cast<int64_t>(m_mediaCodecDecInstance), 0);
////        }
////    }
////#endif
////
////    if (!(m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER)) {
////        if (biterateMode == ENCODE_BITRATE_ABR) {
////            setupVideoEncoderBitrateInCbrMode(m_pVideoEncCtx, videoEncoder, in_videoRes.nImageWidth,
////                                              in_videoRes.nImageHeight, framerate, bps);
////        } else if (biterateMode == ENCODE_BITRATE_CRF) {
////            int crf = static_cast<int>(encodeParams.getInt(VIDEO_ENCODE_CRF));
////            int64_t preset = encodeParams.getInt(VIDEO_ENCODE_PRESET);
////            av_dict_set(&opt, "preset", ies_x264_preset_names[preset], 0);
////            av_dict_set(&opt, "deblock", encodeParams.getString(VIDEO_ENCODE_DEBLOCK)->c_str(), 0);
////#ifdef ENABLE_LIBBYTE264
////            if (TERuntimeConfig::s_bEnableByte264) {
////                std::string *qpoffset = encodeParams.getString(VIDEO_ENCODE_BYTE246_QPOFFSET);
////                if (qpoffset != NULL) {
////                    av_dict_set(&opt, "qpoffset", qpoffset->c_str(), 0);
////                }
////            }
////#endif
////            bool isSetMaxrate = encodeParams.getBool(VIDEO_ENCODE_SET_MAX_BITRATE);
////            int64_t maxBitrate = encodeParams.getInt(VIDEO_ENCODE_MAX_BITRATE);
////            if( isSetMaxrate && maxBitrate > 0 ) {     // 如果采用设置码率策略，且max_rate值合法，则设置最大max_rate；否则采用zerolatency模式  参阅：https://wiki.bytedance.net/pages/viewpage.action?pageId=280690999
////                m_pVideoEncCtx->rc_max_rate = 0.8 * maxBitrate;
////                m_pVideoEncCtx->rc_buffer_size = 0.8 * maxBitrate * 2;      //max_rate可能有20%的码率波动，所以将max_rate设置为0.8倍     参阅：https://wiki.bytedance.net/pages/viewpage.action?pageId=280690999
////
////                std::string crfStr = VESDK::UTIL::to_string(crf+3);     //去掉zerolantacy，码率和psnr均会上升，通过crf放大来平衡      参阅：https://wiki.bytedance.net/pages/viewpage.action?pageId=280690999
////                av_dict_set(&opt, "crf", crfStr.c_str(), 0);
////            } else {
////                av_dict_set(&opt, "tune", "zerolatency", 0);
////
////                std::string crfStr = VESDK::UTIL::to_string(crf);
////                av_dict_set(&opt, "crf", crfStr.c_str(), 0);
////            }
////        } else if (biterateMode == ENCODE_BITRATE_QP) {
////            std::ostringstream os;
////            os << qp;
////            av_dict_set(&opt, "qp", os.str().c_str(), 0);
////        } else if (biterateMode == ENCODE_BITRATE_VBR) {
////            //todo 设置VBR相关参数
////        }
////
////    } else if (m_iFlags & TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER) {
////        setupVideoEncoderBitrateInCbrMode(m_pVideoEncCtx, videoEncoder, in_videoRes.nImageWidth,
////                                          in_videoRes.nImageHeight, framerate, bps);
////#ifdef _WIN32
////        if (codecName == "h264_qsv") {
////            if(!TEFFmpegUtils::checkIntelEncoder(in_videoRes.nImageWidth,
////                in_videoRes.nImageHeight, bps, true))
////                av_dict_set(&opt, "low_power", "false", 0);
////            else
////                av_dict_set(&opt, "low_power", "true", 0);
////            av_dict_set(&opt, "bf", "0", 0);
////        }
////        else if (codecName == "h264_nvenc") {
////            av_dict_set(&opt, "rc", "16", 0);
////        }
////        else if (codecName == "h264_amf") {
////            av_dict_set(&opt, "rc", "1", 0);
////            av_dict_set(&opt, "bf_ref", "false", 0);
////        }
////#endif
////    }
////
////    if( m_bIsCompileSoftInfo ) {
////        m_pVideoEncCtx->flags |= AV_CODEC_FLAG_PSNR;
////    }
////
////    // This will call AVCodec.init()
////    ret = avcodec_open2(m_pVideoEncCtx, videoEncoder, opt ? &opt : 0);
////    if (opt)
////        av_dict_free(&opt);
////
////    if (ret < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "avcodec_open2() failed !");
////        addReportSimpleErrorInfo("addVideoStream TEBundle ", "avcodec_open2() failed !");
////        cleanup();
////        return COMBINE_I64ERROR_WITH_EXT(ret,TER_MEDIA_CODEC_INIT_ERROR);
////    }
////
////    /* copy the stream parameters to the muxer */
////    ret = avcodec_parameters_from_context(m_pVideoStream->codecpar, m_pVideoEncCtx);
////    if (ret < 0) {
////        TELogcat::LogFFMpegError(TAG, ret, "avcodec_parameters_from_context() failed !");
////        addReportSimpleErrorInfo("addVideoStream TEBundle ", "avcodec_parameters_from_context() failed !");
////        cleanup();
////        return COMBINE_I64ERROR_WITH_EXT(ret,AVCODEC_PARAMETERS_FROM_CONTEXT_FAILED);
////    }
////
////    /* Some formats want stream headers to be separate. */
////    if (m_pOutputFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
////        m_pVideoEncCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
////
////    addVideoReportCommonInfo(m_pVideoStream);
////    return TER_OK;
////}
////
////
////void TEFFMpegFileWriter::baseBitrateInfo(const AVCodec *codec,
////                                         unsigned int &baseArea,
////                                         unsigned int &baseBitrate) {
////    baseArea = 1280 * 720;
////    if (codec->id == AV_CODEC_ID_H264)
////        baseBitrate = 4000000;
////    else if (codec->id == AV_CODEC_ID_MPEG4)
////        baseBitrate = 5000000;
////    else
////        baseBitrate = 6000000;
////}
////
////string TEFFMpegFileWriter::getFilePath() { return m_sFilePath; }
////
////void TEFFMpegFileWriter::setupVideoEncoderBitrateInAbrMode(AVCodecContext *videoEncoderCtx,
////                                                           const AVCodec *codec,
////                                                           unsigned int imageWidth,
////                                                           unsigned int imageHeight) {
////    int bitrate;
////    if (!(m_iFlags & TE_AV_FILE_WRITER_FLAG_OPTIMIZED_FOR_SHARING)) {
////        unsigned int baseArea = 0, baseBitrate = 0;
////        baseBitrateInfo(codec, baseArea, baseBitrate);
////        const unsigned int area = imageWidth * imageHeight;
////        bitrate = (int) ((double) area / baseArea * baseBitrate);
////    } else {
////        bitrate = 1500000;
////    }
////
////    videoEncoderCtx->bit_rate = bitrate;
////
////    // In ABR mode, we set rc_min_rate/rc_max_rate/rc_buffer_size to 0
////    videoEncoderCtx->rc_min_rate = 0;
////    videoEncoderCtx->rc_max_rate = 0;
////    videoEncoderCtx->rc_buffer_size = 0;
////}
////
////
////void TEFFMpegFileWriter::setupVideoEncoderBitrateInCbrMode(AVCodecContext *videoEncoderCtx,
////                                                           const AVCodec *codec,
////                                                           unsigned int imageWidth,
////                                                           unsigned int imageHeight, int fps,
////                                                           int bps) {
////    int bitrate, rcBufferSize;
////    bitrate = bps;
////
////    // NOTE: we must set rc_buffer_size once we have set rc_max_rate
////    // FIXME: can we find a more robust way to calculate rate control buffer size?
////    rcBufferSize = bitrate;
////
////    // Set bit_rate/rc_min_rate/rc_max_rate/rc_buffer_size
////    videoEncoderCtx->bit_rate = bitrate;
////    videoEncoderCtx->rc_min_rate = bitrate;
////    videoEncoderCtx->rc_max_rate = bitrate;
////    videoEncoderCtx->rc_buffer_size = rcBufferSize;
////}
////
////
////void TEFFMpegFileWriter::setupX264VideoEncoderBitrateInCrfMode(AVCodecContext *videoEncoderCtx,
////                                                               AVDictionary **opt) {
////    if (m_iFlags & TE_AV_FILE_WRITER_FLAG_OPTIMIZED_FOR_SHARING)
////        av_dict_set(opt, "preset", "veryfast", 0);
////
////    av_dict_set(opt, "crf", "20", 0);
////}
////
////list<unsigned int> TEFFMpegFileWriter::querySupportedInputAudioChannelCount() {
////    list<unsigned int> listSupportedInputAudioChannelCount;
////
////    // Find audio codec
////    const AVCodecID codecId = audioCodecId();
////    AVCodec *audioEncoder = avcodec_find_encoder(codecId);
////    if (!audioEncoder) {
////        VELOGE("Couldn't find audio encoder for codec id=%d!", codecId);
////        return listSupportedInputAudioChannelCount;
////    }
////
////    if (!audioEncoder->channel_layouts) {
////        listSupportedInputAudioChannelCount.push_back(2);
////        return listSupportedInputAudioChannelCount;
////    }
////
////    int i = 0;
////    for (;; i++) {
////        const uint64_t channelLayout = audioEncoder->channel_layouts[i];
////        if (!channelLayout)
////            break;
////
////        const int channelCount = av_get_channel_layout_nb_channels(channelLayout);
////        auto iter = find(listSupportedInputAudioChannelCount.begin(),
////                         listSupportedInputAudioChannelCount.end(),
////                         (unsigned int) channelCount);
////        if (iter == listSupportedInputAudioChannelCount.end())
////            listSupportedInputAudioChannelCount.push_back((unsigned int) channelCount);
////    }
////
////    return listSupportedInputAudioChannelCount;
////}
////
////void TEFFMpegFileWriter::setAllIFrameEncoding(bool value) {
////    m_bAllIFrameEncoding = value;
////}
////
////void TEFFMpegFileWriter::notifyVideoEOS() {
////
////}
////
////void TEFFMpegFileWriter::notifyAudioEOS() {
////
////}
////
////unsigned int TEFFMpegFileWriter::getVideoStreamCount() {
////    return 1;
////}
////
////unsigned int TEFFMpegFileWriter::getAudioStreamCount() {
////    return m_pAudioStreams.size();
////}
////
////unsigned int TEFFMpegFileWriter::queryAudioEncodingFrameSize(unsigned int nAudioStreamIndex) {
////    if (nAudioStreamIndex >= m_pAudioEncCtx.size()) {
////        VELOGE("index out of bound!");
////        return 1024;
////    }
////
////    return m_pAudioEncCtx[nAudioStreamIndex]
////           ? static_cast<unsigned int>(m_pAudioEncCtx[nAudioStreamIndex]->frame_size) : 1024;
////}
////
////int64_t TEFFMpegFileWriter::getTotalWriteAudioTime() const {
////    return m_iTotalWriteAudioTime;
////}
////
////int64_t TEFFMpegFileWriter::getTotalWriteVideoTime() const {
////    return m_iTotalWriteVideoTime;
////}
////
////int TEFFMpegFileWriter::getWriteVideoFrameCount() const {
////    return m_iWriteVideoFrameCount;
////}
////
////int TEFFMpegFileWriter::getWriteAudioFrameCount() const {
////    return m_iWriteAudioFrameCount;
////}
////
////int TEFFMpegFileWriter::getWriterFlags() const {
////    return m_iFlags;
////}
////
////bool TEFFMpegFileWriter::getAudioHw() const {
////    return m_audioHw;
////}
////
////int TEFFMpegFileWriter::getSrcPixelFormat() const {
////    return pixelformat;
////}
////
////void TEFFMpegFileWriter::setReportPtr(TESmartPtr<TEWorkReport> &report) {
////    reportPtr = report;
////}
////
////TESmartPtr<TEWorkReport> TEFFMpegFileWriter::getReportPtr() {
////    return reportPtr;
////}
////
////void TEFFMpegFileWriter::addVideoReportCommonInfo(AVStream *videoStream) {
////    if (videoStream == nullptr || TERuntimeConfig::ve_CompileReport <= 0) {
////        VELOGI("TERuntimeConfig::ve_CompileReport=%d or videoStream is null", TERuntimeConfig::ve_CompileReport);
////        return;
////    }
////
////    if (reportPtr != nullptr) {
////        char data[300] = { 0 };
////        sprintf(data, "codec_id=%d res=(%dx%d) bitrate=%" PRId64 " pix_fmt=%s", videoStream->codecpar->codec_id, videoStream->codecpar->width, videoStream->codecpar->height,
////                videoStream->codecpar->bit_rate, av_get_pix_fmt_name((AVPixelFormat)videoStream->codecpar->format));
////        reportPtr->addCommonInfo("video_codec_info: ", data);
////        addReportSimpleErrorInfo(FLUSH_WRITER, "has no EOF");
////    }
////}
////
////void TEFFMpegFileWriter::addAudioReportCommonInfo(AVStream *audioStream) {
////    if (audioStream == nullptr || TERuntimeConfig::ve_CompileReport <= 0) {
////        VELOGI("TERuntimeConfig::ve_CompileReport=%d audioStream=%p", TERuntimeConfig::ve_CompileReport, (void *)audioStream);
////        return;
////    }
////
////    if (reportPtr != nullptr) {
////        char data[300] = { 0 };
////        sprintf(data, "codec_id=%d channels=%d bitrate=%" PRId64 " format=%d", audioStream->codecpar->codec_id, audioStream->codecpar->channels, audioStream->codecpar->bit_rate,
////                audioStream->codecpar->format);
////        reportPtr->addCommonInfo("audio_codec_info: ", data);
////        addReportSimpleErrorInfo(FLUSH_WRITER, "has no EOF");
////    }
////}
////
////void TEFFMpegFileWriter::addReportSimpleErrorInfo(std::string key, std::string value) {
////    if (reportPtr != nullptr) {
////        reportPtr->addErrorInfo(key, value);
////    }
////}
//
//
////
//// Created by xiangweixin on 2020/8/3.
////
//
//#include "TERecordFFmpegFileWriter.h"
//
//#define TAG "TERecordFFmpegFileWriter"
//
//TERecordFFmpegFileWriter::TERecordFFmpegFileWriter(int flags) : TEFFMpegFileWriter(flags, 0) {
//    m_bOpenDone = false;
//    m_bStartDone = false;
//
//    m_iFlushCount = 2;
//    m_bFlushDone = false;
//
//    m_iAddStreamFlag = NONE_STREAM;
//}
//
//int64_t TERecordFFmpegFileWriter::flushWriter() {
//    std::lock_guard<std::mutex> lck(m_mutex);
//    if (m_bFlushDone == true) {
//        TELogcat::LogW(TAG, "flushWriter has already done.");
//        return TER_RECORD_AV_WRITER_ALREADY_DONE;
//    }
//    m_iFlushCount--;
//    if (m_iFlushCount > 0) {
//        TELogcat::LogI(TAG, "Try to flush once.");
//        return TER_RECORD_AV_WRITER_WAITING_NEXT_FLUSH;
//    }
//    int64_t ret = TEFFMpegFileWriter::flushWriter();
//    m_bFlushDone = true;
//    return ret;
//}
//
//int64_t TERecordFFmpegFileWriter::openWriter(vector<string> filePath) {
//    std::lock_guard<std::mutex> lck(m_mutex);
//    if (m_bOpenDone) {
//        TELogcat::LogW(TAG, "Writer has already been opened.");
//        return TER_RECORD_AV_WRITER_ALREADY_DONE;
//    }
//    int64_t ret = TEFFMpegFileWriter::openWriter(filePath);
//    m_bOpenDone = true;
//    return ret;
//}
//
//int64_t TERecordFFmpegFileWriter::start() {
//    std::unique_lock<std::mutex> lck(m_mutexStart);
//    if (m_bStartDone) {
//        TELogcat::LogW(TAG, "Writer has already been start.");
//        return TER_RECORD_AV_WRITER_ALREADY_DONE;
//    }
//    m_condVar.wait(lck, [this] {
//        return ((m_iAddStreamFlag & ADD_AUDIO_STREAM) && (m_iAddStreamFlag & ADD_VIDEO_STREAM));
//    });
//
//    int64_t ret = TEFFMpegFileWriter::start();
//    m_bStartDone = true;
//    return ret;
//}
//
//int64_t TERecordFFmpegFileWriter::addAudioStream(const STEAudioResolution &sAudioRes) {
//    std::unique_lock<std::mutex> lck(m_mutexStart);
//    int64_t ret = TEFFMpegFileWriter::addAudioStream(sAudioRes);
//    if (ret == TER_OK) {
//        m_iAddStreamFlag |= ADD_AUDIO_STREAM;
//    } else {
//        TELogcat::LogW(TAG, "addAudioStream failed, ret: %lld", ret);
//    }
//    m_condVar.notify_all();
//    lck.unlock();
//    return ret;
//}
//
//int64_t TERecordFFmpegFileWriter::addVideoStream(TEBundle &encodeParams) {
//    std::unique_lock<std::mutex> lck(m_mutexStart);
//    int64_t ret = TEFFMpegFileWriter::addVideoStream(encodeParams);
//    if (ret == TER_OK) {
//        m_iAddStreamFlag |= ADD_VIDEO_STREAM;
//    } else {
//        TELogcat::LogW(TAG, "addVideoStream failed, ret: %lld", ret);
//    }
//    m_condVar.notify_all();
//    lck.unlock();
//    return ret;
//}
