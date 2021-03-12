////
//// Created by bytedance on 10/03/2021.
////
//
//#ifndef CAMERA_UFFMPEGFILEWRITER_H
//#define CAMERA_UFFMPEGFILEWRITER_H
//
//#endif //CAMERA_UFFMPEGFILEWRITER_H
//
//
//#pragma once
//
//
//#ifdef __cplusplus
//extern "C" {
//#include <libavutil/pixdesc.h>
//#endif
//#if defined(__IOS__)
//#include <libavformat/avformat.h>
//#include <TTFFmpeg/libavcodec/videotoolbox.h>
//#include "TEFFMpegConfigData.h"
//#else
//#include <ffmpeg/include/libavformat/avformat.h>
//#endif
//#ifdef __cplusplus
//}
//#endif
//
//#include <vector>
//#include <mutex>
//#include <TEThreadObject.h>
//#include <TEAVFormatContext.h>
//
//#include "TELogcat.h"
//#include "TELightUnknown.h"
//#include "ITEAVFileWriter.h"
//#include "TEFFMpegBufferWriter.h"
//#include "TEWorkReport.h"
//
//#ifdef _WIN32
//#include "TEIntelVPP.h"
//#endif
//
//using namespace std;
//
//
//#define TE_AV_FILE_WRITER_FLAG_OPTIMIZED_FOR_SHARING        0x1
//#define TE_AV_FILE_WRITER_FLAG_ENABLE_HW_VIDEO_ENCODER      0x2
//#define TE_AV_FILE_WRITER_FLAG_ENABLE_HW_AUDIO_ENCODER      0x4
//#define TE_AV_FILE_WRITER_FLAG_REMUX_AUDIO                  0x8
//#define TE_AV_FILE_WRITER_FLAG_GIF                          0x10
//#define TE_AV_FILE_WRITER_FLAG_SINGLE_VIDEO                 0x20
//#define TE_AV_FILE_WRITER_FLAG_BUFFER                       0x40
//#define TE_AV_FILE_WRITER_FLAG_REMUX_VIDEO                  0x80
//#define TE_AV_FILE_WRITER_FLAG_COMPILE_SOFT_INFO            0x100
//#define TE_AV_FILE_WRITER_FLAG_ALPHA_MOV                    0x200
//#define TE_AV_FILE_WRITER_FLAG_MPEG4                        0x400
//
//#define TE_AV_FILE_AUDIO_FLAG_HW_ENCODER                    0x1
//#define TE_AV_FILE_AUDIO_FLAG_AAC_ENCODER                   0x2
//
//#define TE_FLUSH_MAX_WAIT_TIME_IN_MS                        100
//
//#define TE_MSG_FILE_WRITER_PKT                              0xE
//
//#define TE_AV_FILE_WRITER_MAX_AUDIO_BUFF_SIZE               (1024*1024) //1M
//
//#define TE_AV_MOV_HEADER_SIZE                               44
//
//#define AAC_OUTPUT_MODE "0"
//
//#define FLUSH_WRITER "flush_writer"
//
//typedef struct TEIOBufferContext {
//    bool            readytoWriteTrailer;
//    int64_t         currentWritten;
//    int64_t         headerCacheBufSize;
//    int64_t         trailerCacheBufSize;
//    int64_t         trailerCacheBufStart;
//    int64_t         trailerCacheBufPos;
//    AVIOContext*    avioCtx;
//    uint8_t*        headerCacheBuffer;
//    uint8_t*        trailerCacheBufer;
//    TE_IO_SEND_BUF  sendBufFunc;
//    void*           userData;
//    FILE*           file;
//
//    TEIOBufferContext () {
//        readytoWriteTrailer = false;
//        currentWritten = 0;
//        headerCacheBufSize = 0;
//        trailerCacheBufSize = 0;
//        trailerCacheBufPos = 0;
//        trailerCacheBufStart = 0;
//
//        avioCtx = nullptr;
//
//        trailerCacheBufer = nullptr;
//        headerCacheBuffer = nullptr;
//
//        sendBufFunc = nullptr;
//        userData = nullptr;
//
//    };
//} TEIOBufferContext;
//
//static const char * const ies_x264_preset_names[] = { "ultrafast", "superfast", "veryfast", "faster", "fast", "medium", "slow", "slower", "veryslow", "placebo", 0 };
//
//class ThreadWriteAVPacket : public TEThreadObject {
//public:
//    explicit ThreadWriteAVPacket() {
//        setThreadName((char *) "FW");
//    }
//
//    int getMsgQSize() {return getMessageQueueSize();}
//protected:
//    virtual TEResult customEvent(TEMsg *) override;
//
//private:
//    int m_nPktCnt = 0;
//};
//
//class TEFFmpegVideoPacket;
////bool
////createFFMpegFileWriter(const string &in_filePath, int in_flags, ITEAVFileWriter **out_ppIWriter);
//
//class TEFFMpegFileWriter : public ITEAVFileWriter, public TELightUnknown {
//
//public:
//    TE_DECLARE_IUNKNOWN
//
//    virtual bool nonDelegatingQueryInterface(REFIID in_rIID, void **out_ppv) override ;
//
//    TEFFMpegFileWriter(int flags, int64_t audioFlags);
//
//    virtual ~TEFFMpegFileWriter() override ;
//
//    virtual int64_t openWriter(vector<string> filePath) override ;
//
//    virtual list<ETEPixelFormat> querySupportedHostInputVideoPixelFormat(char const* name = nullptr) override ;
//
//    virtual list<ETEAudioSampleFormat> querySupportedInputAudioSampleFormat() override ;
//
//    virtual list<unsigned int> querySupportedInputAudioChannelCount() override ;
//
//    virtual int64_t start() override ;
//
//    virtual int64_t writeVideoFrame(ITEVideoFrame *in_pIVideoFrame,
//                                    int64_t in_streamTime) override ;
//
//    virtual int64_t writeVideoPacket(ITEVideoFrame *in_pIVideoPacket, int64_t in_streamTime) override;
//
//    virtual int64_t
//    writeAudioSamples(ITEAudioSamples *pAudioSamplesArray[], int64_t i64StreamTimeArray[]) override ;
//
//    virtual int64_t encodeAudioSamples(ITEAudioSamples *pAudioSamples, int64_t i64StreamTime, ITEAudioSamples **ppAudioPackets, bool &sendAgain) override;
//
//    virtual bool haveAudioAsc(int index) override;
//
//    virtual int64_t muxAudioPackets(ITEAudioSamples *pAudioPackets, bool needUnRef) override;
//
//    virtual int64_t flushWriter() override ;
//
//    virtual int64_t addVideoStream(TEBundle &encodeParams) override ;
//
//    virtual int64_t addVideoStream(string sRemuxPath) override ;
//
//    virtual int64_t addAudioStream(const STEAudioResolution &sAudioRes) override ;
//
//    virtual int64_t addAudioStream(string sRemuxPath) override ;
//
//    virtual int64_t getVideoAssumeDuration() override;
//
//    virtual bool isSupportGPUInputVideo() override ;
//
//    virtual void setAllIFrameEncoding(bool value) override ;
//
//    virtual void notifyVideoEOS() override ;
//
//    virtual void notifyAudioEOS() override ;
//
//    virtual unsigned int getVideoStreamCount() override ;     // always 1
//
//    virtual unsigned int getAudioStreamCount() override ;
//
//    virtual unsigned int queryAudioEncodingFrameSize(unsigned int nAudioStreamIndex) override ;
//
//    virtual int getWriteVideoFrameCount() const override ;
//
//    virtual int getWriteAudioFrameCount() const override ;
//
//    virtual int64_t getTotalWriteAudioTime() const override ;
//
//    virtual int64_t getTotalWriteVideoTime() const override ;
//
//    virtual int getSrcPixelFormat() const override;
//
//    virtual int getWriterFlags() const override ;
//
//    virtual bool getAudioHw() const override;
//
//    virtual bool setUserMetadata(const map<string, string> &metaData) override ;
//
//    /*
//     * @brief set file writer buffer mode params
//     * @param send_buf buf callback function, used to send current muxer buffer to up level model
//     */
//    virtual bool setMuxerBufferModeParams(TE_IO_SEND_BUF send_buf, void* userData) override ;
//
//    virtual void setSharedGLProcessQueue(ITETaskQueue *queue) override { m_glProcessQueue = queue; }
//
//    virtual void setMediaCodecDecInstance(void *instance) override { m_mediaCodecDecInstance = instance; }
//
//#if defined(__IOS__)
//    virtual void setConfigData(const TEFFMpegVTConfig &config) override {
//        m_pVTConfigData = config;
//    }
//    virtual void setCreationDateString(std::string dateString) override {
//        m_creationDate = dateString;
//    }
//#endif
//
//protected:
//    virtual int writeAVPacketSafe(AVPacket *pkt, int type);
//
//    virtual void cleanup();
//
//    virtual bool isValid() const;
//
//    bool copyGlobalMetadata(AVFormatContext *videoEncoderCtx, const std::string &srcPath);
//
//    bool copyUserMetadata(AVFormatContext *videoEncoderCtx, const map<string, string> &metaData);
//
//    int64_t flushVideo();
//
//    int64_t flushAudio();
//
//    AVCodecID videoCodecId() const;
//
//    virtual AVCodecID audioCodecId() const;
//
//    void setupVideoEncoderBitrateInCbrMode(AVCodecContext *videoEncoderCtx,
//                                           const AVCodec *codec,
//                                           unsigned int imageWidth,
//                                           unsigned int imageHeight,
//                                           int fps, int bps);
//
//    void baseBitrateInfo(const AVCodec *codec, unsigned int &baseArea, unsigned int &baseBitrate);
//
//    void setupVideoEncoderBitrateInAbrMode(AVCodecContext *videoEncoderCtx, const AVCodec *codec,
//                                           unsigned int imageWidth, unsigned int imageHeight);
//
//    void setupX264VideoEncoderBitrateInCrfMode(AVCodecContext *videoEncoderCtx, AVDictionary **opt);
//
//    string getFilePath() override ;
//
//    void updateEditorInfo();
//
//    int64_t addVideoStreamForMux(TEBundle &muxParams);
//
//    TEResult setupExtradataForStream(TEFFmpegVideoPacket *pkt);
//
//    void setReportPtr(TESmartPtr<TEWorkReport> &report) override;
//
//    TESmartPtr<TEWorkReport> getReportPtr() override;
//
//    void addVideoReportCommonInfo(AVStream *videoStream);
//
//    void addAudioReportCommonInfo(AVStream *audioStream);
//
//    void addReportSimpleErrorInfo(std::string key, std::string value);
//
//#ifdef _WIN32
//    private:
//    std::unique_ptr<TEIntelVPP> m_IntelVPP;
//#endif
//
//protected:
//    int m_writeMode;
//    int m_iCodecID;
//    bool m_bProfileIsBaseline;
//    int m_iFlags;
//    int64_t m_audioFlags = 0;
//    string m_sFilePath;
//    bool m_bStarted;
//    bool m_bIsFlushing;
//    bool m_bIsSendFull;
//    bool m_bIsValid;
//    bool m_bUseMultiThreadEncoding;
//    map<string, string> m_pMapUserMetaData;
//    ETEPixelFormat m_ePixFmt = TEPixFmt_None;
//
//    TESmartPtr<TEWorkReport> reportPtr = nullptr;
//#if defined(__IOS__)
//    TEFFMpegVTConfig m_pVTConfigData;
//    std::string m_creationDate;
//#endif
//
//    //ffmpeg related
//    AVIOContext *m_pWriteIOCtx; //write IO Context, used by ffmpeg in buffer case
//    AVFormatContext *m_pOutputFormatCtx; // Output format context used by ffmpeg
//    AVStream *m_pVideoStream;            // Video stream, may be null
//    AVCodecContext *m_pVideoEncCtx;   //AVCodecContext in AVStream is "deprecated"
//    FILE* mFileWriter;          //write file when fast upload at the same time
//
//    vector<AVStream *> m_pAudioStreams; // Audio stream array
//    vector<AVCodecContext *> m_pAudioEncCtx;   //AVCodecContext in AVStream is "deprecated"
//
//    // video hw encoder need copy extradata by oneself
//    volatile bool m_videoEncodeExtradataReady = false;
//    volatile bool m_audioHw = false;
//
//    uint8_t *m_pBgrData;
//    AVFrame *m_pEncVideoFrame;
//    AVFrame *m_pEncAudioFrame;
//    AVPacket *m_pEncVideoPkt;
//    AVPacket *m_pEncAudioPkt;
//
//    //encoding status
//    int64_t m_iFrameNum;
//    int64_t m_i64LastPts;
//    int64_t m_i64LastDts{ AV_NOPTS_VALUE };
//    int64_t m_i64LastWriteVideoPts;
//    bool m_bFirstFrameEncoded;
//    int64_t m_i64FisrtFrameEncodePts;
//    int64_t m_i64FisrtFrameEncodeDts;
//    volatile bool m_bFileHeaderReady;
//    int m_iWriteVideoFrameCount = 0;
//    int m_iWriteVideoFrameNULLCount = 0;
//    int m_iWriteAudioFrameCount = 0;
//    // 端监控统计时长用 us
//    int64_t m_iTotalWriteAudioTime = 0;
//    int64_t m_iTotalWriteVideoTime = 0;
//
//    //encoding param
//    bool m_bAllIFrameEncoding;
//    bool m_bIsEnableMoovFront = false;
//    // use qtrle codec
//    bool m_bAlphaMovEncoding = false;
//    string m_strGlobalMetaData;
//
//    std::mutex m_writeLock;
//
////    ThreadWriteAVPacket *m_writeThread;
//    std::list<AVPacket *> m_listAudioPkt;
//    std::list<AVPacket *> m_listVideoPkt;
//    int m_nAudioBuffSize;
//
//    //for remux
//    TEAVFormatContext *m_pInputFormatCtx;
//    int         m_nAudioStreamIndex;
//    bool        m_bRemuxAudio;
//
//    TEIOBufferContext* m_pTEIOBufCtx;
//    //for remux video pts and dts adjustment
//    bool m_bIsFirstRemuxVideo;
//    int64_t m_remuxVideoPtsStart;
//    int64_t m_remuxVideoDtsStart;
//    int64_t m_iAssumedVPktDuration;
//
//    TEAVFormatContext *m_pInputFormatCtxV;
//    int         m_nVideoStreamIndex;
//    bool        m_bRemuxVideo;
//
//    int64_t m_remuxLastFrameDts;
//
//    bool  m_bIsVideoPacketReceived = false;
//
//    ITETaskQueue *m_glProcessQueue = nullptr;
//
//    void *m_mediaCodecDecInstance = nullptr;
//
//    bool        m_bIsCompileSoftInfo = false;
//
//    int frameIndex = 0;
//
//    int pixelformat = -1;
//
//    int64_t _writeVideoFrame(ITEVideoFrame *in_pIVideoFrame,
//                             int64_t in_streamTime);
//    int64_t _writeAudioSamples(ITEAudioSamples *pAudioSamplesArray[], int64_t i64StreamTimeArray[]);
//
//    int64_t _flushVideo();
//
//    //used in buffer case
//    bool _createIOContext(int cachebufferSize);
//
//    //used in buffer case
//    void _flushIOBufCache();
//
//    int64_t _flushAudio();
//
//    virtual int64_t _writeAVFormatHeader();
//};
