package com.zxx.camera.mediacodec;

import android.icu.number.UnlocalizedNumberFormatter;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.os.Build;

public class MediaCodecUtils {

    public static MediaCodecInfo getMedaiCodecInfo(String strCodeName){
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP ? getMedaiCodecInfo21(strCodeName) : getMedaiCodecInfoLow(strCodeName);
    }

    public static MediaCodecInfo getMedaiCodecInfoLow(String strCodeName){
        int nbCodecs = MediaCodecList.getCodecCount();
        for(int i = 0;i < nbCodecs;i++ ){
            MediaCodecInfo mediaCodecInfo = MediaCodecList.getCodecInfoAt(i);
            if(!mediaCodecInfo.isEncoder()){
                continue;
            }
            String name = mediaCodecInfo.getName();
            if(name.startsWith("OMX.google.") || name.startsWith("OMX.Nvidia.") || name.equals("OMX.TI.DUCATI1.VIDEO.H264E")){
                continue;
            }
            String[] types = mediaCodecInfo.getSupportedTypes();
            for(int j = 0; j <  types.length;j++){
                if(types[j].equalsIgnoreCase(strCodeName))
                    return mediaCodecInfo;
            }
        }
        return null;
    }


    protected static MediaCodecInfo getMedaiCodecInfo21(String strCodeName) {
        MediaCodecList mediaCodecList = new MediaCodecList(MediaCodecList.ALL_CODECS);
        MediaCodecInfo[] mcArray = mediaCodecList.getCodecInfos();
        if (mcArray == null || mcArray.length == 0) {
            return null;
        }
        for (MediaCodecInfo mediaCodecInfo : mcArray) {
            if (mediaCodecInfo == null || !mediaCodecInfo.isEncoder()) {
                continue;
            }
            String name = mediaCodecInfo.getName();
            if (name.startsWith("OMX.google.") || name.startsWith("OMX.Nvidia.") || name.equals("OMX.TI.DUCATI1.VIDEO.H264E")) {
                continue;
            }
            String[] types = mediaCodecInfo.getSupportedTypes();
            for (int i = 0; i < types.length; i++) {
                if (types[i].equalsIgnoreCase(strCodeName)) {
                    return mediaCodecInfo;
                }
            }
        }
        return null;
    }


}
