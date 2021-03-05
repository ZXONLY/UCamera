package com.zxx.unique.mediacodec;

import android.annotation.TargetApi;
import android.media.Image;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.os.ConditionVariable;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.Surface;

import androidx.annotation.NonNull;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.concurrent.LinkedBlockingQueue;

@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public abstract class MediaCodecEncoder {
    protected static final String TAG = MediaCodecEncoder.class.getName();

    protected enum EncoderStatus{
        UNSET,
        INITED,
        STARTED,
        STOPPED,
        RELEASED
    }

    protected MediaCodecSettings mEncoderSeetings = null;
    protected MediaCodec mMediaCodec = null;
    protected MediaFormat mMediaFormat = null;
    protected Surface mInputSurface = null;
    protected MediaCodecEncoderCallback mMediaCodecEncoderCallback = null;
    protected HandlerThread mEncodeThread;
    protected Handler mEncodeHandler;
    public int mInputFrames;
    public int mOutputFrames;
    public boolean mInputEOS = false;

    protected LinkedBlockingQueue<Long> mPTSQueue = new LinkedBlockingQueue<>();
    protected volatile EncoderStatus mStatus = EncoderStatus.UNSET;
    protected byte[] mCSDData;

    protected MediaCodecEncoder(){
        mEncodeThread = new HandlerThread("CodecEncoder");
        mEncodeThread.start();
        mEncodeHandler = new Handler(mEncodeThread.getLooper(),null);//MediaCodecEncoderCallback
    }


    protected abstract int configRunningMode();

    public void setEncoderCallback(MediaCodecEncoderCallback callback){
        mMediaCodecEncoderCallback = callback;
    }

    public static MediaCodecEncoder createEncoder(int runningmode){
        return new MediaCodecAsyncEncoder();
    }

    public Surface getInputSurface(){
        return mInputSurface;
    }

    public MediaFormat getMediaFormat(){
        return mMediaFormat;
    }


    public int initEncoder(MediaCodecSettings settings){
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN_MR2){
            return MediaCodecResult.CODEC_UNSUPPORTED;
        }
        if(settings.getmBitRateMode() < MediaCodecSettings.BITRATE_MODE_CQ
        || settings.getmBitRateMode() > MediaCodecSettings.BITRATE_MODE_CBR){
            Log.e(TAG,"Dont support bitrate mode " + settings.getmBitRateMode());
            return MediaCodecResult.CODEC_INVALID_PARAM;
        }

        if(settings.getmWidth() <= 0 || settings.getmHeight() <= 0){
            return MediaCodecResult.CODEC_INVAILD_VIDEO_SIZE;
        }

        Message msg = new Message();
        msg.what = EncodeHandlerCallback.MSG_INIT;
        ConditionVariable lock = new ConditionVariable();
        lock.close();
        msg.obj = new EncodeSettingsWrapper(mEncoderSeetings,lock);
        mEncodeHandler.sendMessage(msg);

        if(!lock.block(500000)){
            Log.e(TAG,"initEncoder timeout");
            return MediaCodecResult.CODEC_INITENCODER_FAIL;
        } else if(mStatus != EncoderStatus.INITED){
            return MediaCodecResult.CODEC_INITENCODER_FAIL;
        }
        return MediaCodecResult.CODEC_OK;

    }

    public int startEncode(){
        int ret = handleStartEncoder();
        return ret;
    }

    public int stopEncode(){
        handleStopEncoder();
        return MediaCodecResult.CODEC_OK;
    }

    public int releaseEncode(){
        handleReleaseEncoder();

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2){
            mEncodeThread.quitSafely();
        }else {
            mEncodeThread.quit();
        }
        mEncodeHandler = null;
        mEncodeThread = null;
        mInputFrames = 0;
        mOutputFrames = 0;
        mCSDData = null;
        mMediaFormat = null;
        mStatus = EncoderStatus.UNSET;
        return MediaCodecResult.CODEC_OK;
    }

    public int restartEncode(){
        stopEncode();
        int ret = initEncoder(mEncoderSeetings);
        if(ret < 0){
            return ret;
        }
        startEncode();
        return MediaCodecResult.CODEC_OK;
    }

    public int encode(MediaFrame frame) throws Exception {
        if(mStatus != EncoderStatus.STARTED){
            Log.e(TAG,"Cannot encode before starting encode.");
            return  MediaCodecResult.CODEC_INVALID_STATUS;
        }

        if(mMediaCodecEncoderCallback == null){
            Log.e(TAG,"encoder caller is null! P;ease encoder callback");
            return  MediaCodecResult.CODEC_NO_CALLBACK;
        }

        if(!frame.isValid() && !frame.isEndFrame){
            Log.e(TAG,frame.toString());
            return MediaCodecResult.CODEC_INVALID_PARAM;
        }

        int ret = MediaCodecResult.CODEC_OK;
        try{
            if(mEncoderSeetings.useSurfaceInput()){
                ret = encodeTexture(frame);
            }else{
                ret = doEncodeBuffer(frame);
            }
        }catch (Exception e){
            throw e;
        }

        if(ret == MediaCodecResult.CODEC_OK){
            Log.i(TAG,"encode... " +frame + " index: " + mInputFrames);
            mInputFrames ++;
            mPTSQueue.offer(frame.pts);
            if(frame.isEndFrame){
                mInputEOS = true;
            }
        }
        return ret;
    }

    protected abstract int doEncodeBuffer(MediaFrame frame) throws Exception;

    protected int encodeTexture(MediaFrame frame){
        if(mStatus != EncoderStatus.STARTED){
            return MediaCodecResult.CODEC_INVALID_STATUS;
        }
        if(frame.isValid()){
            mMediaCodecEncoderCallback.onFillInputSurface(frame);
        }
        if(frame.isEndFrame){
            Log.i(TAG,"signal end of stream... pts: " + frame.pts);
            mMediaCodec.signalEndOfInputStream();
        }
        return MediaCodecResult.CODEC_OK;
    }

    protected  void receiveEncodedData(MediaCodec codec, int index, MediaCodec.BufferInfo bufferInfo){
        if(mStatus != EncoderStatus.STARTED){
            Log.w(TAG,"the mediaCodec status is not started, the status is " + mStatus);
            return;
        }

        if((bufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) !=0){
            ByteBuffer outputBuffer = codec.getOutputBuffer(index);
            mCSDData = new byte[bufferInfo.size];
            outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
            outputBuffer.get(mCSDData);
            codec.releaseOutputBuffer(index,false);
            return;
        }

        Log.i(TAG,"out frame index: " + mOutputFrames);
        mOutputFrames ++;

        MediaFrame outputFrame = new MediaFrame();

        ByteBuffer outputBuffer = codec.getOutputBuffer(index);
        outputBuffer.position(bufferInfo.offset);
        outputBuffer.limit(bufferInfo.offset + bufferInfo.size);

        if(bufferInfo.size > 0){
            outputFrame.data = new byte[bufferInfo.size];
            outputBuffer.get(outputFrame.data);
        }else if((bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) == 0 ){
            Log.e(TAG,"frame.mBufferInfo.size <= 0. size: " + bufferInfo.size);
        }

        codec.releaseOutputBuffer(index,false);

        boolean isKeyFrame = (bufferInfo.flags & MediaCodec.BUFFER_FLAG_KEY_FRAME) != 0;
        if(isKeyFrame){

            if(mCSDData !=null && outputFrame.data != null && outputFrame.data.length > mCSDData.length +4 && (outputFrame.data[4]==mCSDData[4])){
                byte[] clipData = new byte[outputFrame.data.length - mCSDData.length];
                System.arraycopy(outputFrame.data,mCSDData.length,clipData,0,clipData.length);
                outputFrame.data = clipData;
            }
        }

        Long dts = mPTSQueue.poll();
        if(dts == null) {
            Log.e(TAG," send frames' count is not equal receive frames's count!!!!!");
            return;
        }
        boolean endFrame = (bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM)!=0;
        if(endFrame){
            outputFrame.pts = dts;
        }else {
            outputFrame.pts = bufferInfo.presentationTimeUs;
        }
        outputFrame.dts = dts;

        outputFrame.isEndFrame = endFrame;
        outputFrame.isKeyFrame = isKeyFrame;
        if(mMediaCodecEncoderCallback !=null){
            mMediaCodecEncoderCallback.onEncodedFrameAvailable(outputFrame);
        }
        return;
    }


    protected static class EncodeHandlerCallback implements Handler.Callback{
        public static final int MSG_INIT = 1;
        public static final int MSG_START = 2;
        public static final int MSG_STOP = 3;
        public static final int MSG_RELEASE = 4;

        private WeakReference<MediaCodecEncoder> mMediaEncoder;

        public EncodeHandlerCallback(MediaCodecEncoder encoder){
            mMediaEncoder = new WeakReference<>(encoder);
        }

        @Override
        public boolean handleMessage(@NonNull Message msg) {
            int what = msg.what;
            Object obj = msg.obj;

            MediaCodecEncoder mediaCodecEncoder = mMediaEncoder.get();

            switch (what){
                case MSG_INIT:
                    Log.d(TAG,"init ...");
                    EncodeSettingsWrapper encodeSettingsWrapper = (EncodeSettingsWrapper) obj;
                    mediaCodecEncoder.handleInitEncoder(encodeSettingsWrapper.encoderSettings);
                    encodeSettingsWrapper.lock.open();
                    break;
                case MSG_START:
                    Log.d(TAG,"start ...");
                    mediaCodecEncoder.handleStartEncoder();
                    break;
                case MSG_STOP:
                    Log.d(TAG,"stop ...");
                    mediaCodecEncoder.handleStopEncoder();
                    break;
                case MSG_RELEASE:
                    Log.d(TAG,"release ...");
                    mediaCodecEncoder.handleReleaseEncoder();
                    return  true;
            }

            return false;
        }
    }

    protected int handleInitEncoder(MediaCodecSettings encoderSettings){
        mEncoderSeetings = encoderSettings;
        try{
            int ret = createMediaCodec();
            if(ret < 0){
                Log.e(TAG,"create encoder fail: " + ret);
                return ret;
            }

            MediaCodecInfo codecInfo = mMediaCodec.getCodecInfo();
            MediaCodecInfo.CodecCapabilities codecCapabilities = codecInfo.getCapabilitiesForType(mEncoderSeetings.getmMimeType());

            MediaFormat format = MediaFormat.createVideoFormat(mEncoderSeetings.getmMimeType(),mEncoderSeetings.getmWidth(),mEncoderSeetings.getmHeight());
            format.setInteger(MediaFormat.KEY_COLOR_FORMAT,mEncoderSeetings.getmInputColorFormat());
            format.setInteger(MediaFormat.KEY_FRAME_RATE,mEncoderSeetings.getmFrameRate());
            format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL,mEncoderSeetings.getmIFrameInternal());

            MediaCodecInfo.CodecProfileLevel profileLevel = findBestMatchedProfile(codecCapabilities,mEncoderSeetings.getmEncodeProfile());

            if(profileLevel != null){
                format.setInteger(MediaFormat.KEY_PROFILE,profileLevel.profile);
                format.setInteger(MediaFormat.KEY_LEVEL,profileLevel.level);
            }

            int bestBitRate = adjustBitRate(mEncoderSeetings.getmBitRate(),profileLevel);

            format.setInteger(MediaFormat.KEY_BIT_RATE,bestBitRate);

            //Set Bitrate Mode ----------------
            if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP){
                if(codecCapabilities.getEncoderCapabilities().isBitrateModeSupported(mEncoderSeetings.getmBitRateMode())){
                    format.setInteger(MediaFormat.KEY_BITRATE_MODE,mEncoderSeetings.getmBitRateMode());
                }else {
                    Log.w(TAG,"codec doesn't support Bitrate MOde: " + mEncoderSeetings.getmBitRateMode());
                }
            }

            ret = configRunningMode();

            if(ret != 0){
                Log.e(TAG,"configRunningMode fail, ret = " + ret);
                return ret;
            }

            mMediaCodec.configure(format,null,null,MediaCodec.CONFIGURE_FLAG_ENCODE);

            if(mEncoderSeetings.useSurfaceInput()){
                mInputSurface = mMediaCodec.createInputSurface();
            }


            mStatus = EncoderStatus.INITED;

        }catch (Exception e){
            mStatus = EncoderStatus.UNSET;
        }
        return MediaCodecResult.CODEC_OK;
    }

    protected int handleStartEncoder(){
        if(mStatus != EncoderStatus.INITED){
            Log.e(TAG,"START encoder with invaild status. Current status: " + mStatus);
            return MediaCodecResult.CODEC_INVALID_STATUS;
        }
        Log.i(TAG,"start encode ...");
        mMediaCodec.start();
        mStatus = EncoderStatus.STARTED;
        return MediaCodecResult.CODEC_OK;
    }

    protected int handleStopEncoder(){
        if(mStatus != EncoderStatus.STARTED){
            Log.e(TAG,"Stop encoder with invaild status. Current status: " + mStatus);
            return MediaCodecResult.CODEC_INVALID_STATUS;
        }
        Log.i(TAG,"Stop encoder ....");
        int ret = MediaCodecResult.CODEC_OK;
        mStatus  = EncoderStatus.STOPPED;
        if(mMediaCodec != null){
            mMediaCodec.stop();
        }
        return ret;
    }

    protected void handleReleaseEncoder(){
        if(mStatus == EncoderStatus.UNSET || mStatus == EncoderStatus.RELEASED){
            Log.e(TAG,"release encoder with invaild status. Current status: " + mStatus);
            return;
        }
        Log.e(TAG,"release encoder...");
        if(mStatus != EncoderStatus.STOPPED);{
            stopEncode();
        }

        if(mInputSurface != null){
            mInputSurface.release();
            mInputSurface = null;
        }

        if(mMediaCodec != null){
            mMediaCodec.release();
        }

        mEncoderSeetings = null;
        mMediaCodec = null;

    }


    //---------------Inner Methods
    protected int createMediaCodec(){
        if(mStatus != EncoderStatus.UNSET){
            return MediaCodecResult.CODEC_INVALID_STATUS;
        }

        MediaCodecInfo mediaCodecInfo = MediaCodecUtils.getMedaiCodecInfo(mEncoderSeetings.getmMimeType());
        if(mediaCodecInfo == null){
            Log.e(TAG,"No invalid codec!");
            return MediaCodecResult.CODEC_INITENCODER_FAIL;
        }

        if(configColorFormat(mediaCodecInfo) != MediaCodecResult.CODEC_OK){
            return MediaCodecResult.CODEC_INITENCODER_FAIL;
        }

        try{
            mMediaCodec = MediaCodec.createEncoderByType(mEncoderSeetings.getmMimeType());
        }catch (Exception e){
            Log.e(TAG,"createByteCodecName throw exception : " + e.toString());
            return MediaCodecResult.CODEC_INITENCODER_FAIL;
        }

        String strCodec = mMediaCodec.getCodecInfo().getName();
        if(!strCodec.equals(mediaCodecInfo.getName())){
            Log.w(TAG,"Donot find proper codec, find " + strCodec +"; require: " +  mediaCodecInfo.getName());
            if(!strCodec.startsWith("OMX.google")){
                Log.w(TAG,"Update codec: " + strCodec);
            }else{
                return MediaCodecResult.CODEC_CODECINFO_ERROR;
            }
        }else {
            Log.i(TAG,"Create codec: " + strCodec);
        }

        return MediaCodecResult.CODEC_OK;

    }

    protected int feedEncode(MediaFrame inputFrame, int index) throws Exception{
        if(mStatus != EncoderStatus.STARTED){
            return MediaCodecResult.CODEC_INVALID_STATUS;
        }
        if(inputFrame == null){
            Log.e(TAG,"Frame is null, frame number: " + mInputFrames);
            return MediaCodecResult.CODEC_INVALID_PARAM;
        }
        if(inputFrame.isValid()){
            Image image = mMediaCodec.getInputImage(index);
            int pixelStride = 1;
            if(image != null){
                pixelStride = image.getPlanes()[1].getPixelStride();
            }
            ByteBuffer byteBuffer = mMediaCodec.getInputBuffer(index);
            byteBuffer.clear();
            byte[] datas;
            if(pixelStride == 2){
                datas = new byte[inputFrame.data.length];
                int k = 0;
                for(int i = 0;i< mEncoderSeetings.getmHeight();i++){
                    for(int j = 0; j < mEncoderSeetings.getmWidth(); j ++ ){
                        int t = i * mEncoderSeetings.getmWidth() + j;
                        datas[k] = datas[t];
                        k++;
                    }
                }

                for(int i = 0; i < mEncoderSeetings.getmHeight() / 4;i++){
                    for(int j = 0; j < mEncoderSeetings.getmWidth(); j++){
                        int t = mEncoderSeetings.getmWidth() * mEncoderSeetings.getmHeight() + (i * mEncoderSeetings.getmWidth() + j);
                        datas[k] = inputFrame.data[t];
                        datas[k+1] = inputFrame.data[t + mEncoderSeetings.getmWidth() * mEncoderSeetings.getmHeight() /4];
                        k += 2;
                    }
                }
            }else {
                datas = inputFrame.data;
            }

            byteBuffer.put(datas);
            byteBuffer.position(0);
            byteBuffer.limit(datas.length);
            mMediaCodec.queueInputBuffer(index,0,inputFrame.data.length,inputFrame.pts,0);
        }else if(inputFrame.isEndFrame){
            Log.i(TAG,"signal end of stream");
            mMediaCodec.queueInputBuffer(index,0,0,inputFrame.pts,MediaCodec.BUFFER_FLAG_END_OF_STREAM);
        }else {
            Log.e(TAG,"invalid input frame: " + inputFrame);
            return MediaCodecResult.CODEC_INVALID_PARAM;
        }
        return MediaCodecResult.CODEC_OK;
    }

    protected int configColorFormat(MediaCodecInfo codecInfo){
        MediaCodecInfo.CodecCapabilities capabilities = codecInfo.getCapabilitiesForType(mEncoderSeetings.getmMimeType());

        if(capabilities.colorFormats == null){
            return MediaCodecResult.CODEC_GETCOLORFORMAT_FAIL;
        }

        for(int i = 0; i < capabilities.colorFormats.length;i++ ){
            if(capabilities.colorFormats[i] == mEncoderSeetings.getmInputColorFormat()){
                return MediaCodecResult.CODEC_OK;
            }
        }
        return MediaCodecResult.CODEC_FAIL;
    }

    protected static class EncodeSettingsWrapper{
        MediaCodecSettings encoderSettings;
        ConditionVariable lock;

        public EncodeSettingsWrapper(MediaCodecSettings settings,ConditionVariable lock){
            this.encoderSettings = settings;
            this.lock = lock;
        }

    }

    static MediaCodecInfo.CodecProfileLevel findBestMatchedProfile(MediaCodecInfo.CodecCapabilities codecCapabilities,
                                                                   int desiredProfile){
        if(Build.VERSION.SDK_INT < Build.VERSION_CODES.N){
            return null;
        }
        MediaCodecInfo.CodecProfileLevel bestProfile = null;
        for(MediaCodecInfo.CodecProfileLevel profileLevel : codecCapabilities.profileLevels ){
            if(profileLevel.profile == desiredProfile){
                bestProfile = profileLevel;
                break;
            }
        }
        return  bestProfile;
    }

    static int adjustBitRate(int maxBitRate, MediaCodecInfo.CodecProfileLevel profileLevel){
        if(profileLevel == null){
            return maxBitRate;
        }

        int bestBitRate = maxBitRate;
        switch(profileLevel.profile){
            case MediaCodecInfo.CodecProfileLevel.AVCProfileHigh:
                Log.i(TAG,"Set High Profile");
                bestBitRate = (int )(maxBitRate * 0.75f);
                break;
            case MediaCodecInfo.CodecProfileLevel.AVCProfileMain:
            case MediaCodecInfo.CodecProfileLevel.HEVCProfileMain:
                Log.i(TAG,"Set Main Profilr");
                bestBitRate = (int)(maxBitRate * 0.85f);
                break;
        }
        return bestBitRate;
    }
}
