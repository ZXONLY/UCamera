package com.zxx.unique.camera;

import android.os.Parcel;
import android.os.Parcelable;

import com.zxx.unique.mediacodec.MediaFrame;

public class UqCameraSettings {

    public enum CAMERA_FACING_ID implements Parcelable {
        FACING_BACK,
        FACING_FRONT;

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeInt(ordinal());
        }

        @Override
        public int describeContents() {
            return 0;
        }

        public static final Creator<CAMERA_FACING_ID> CREATOR = new Creator<CAMERA_FACING_ID>() {
            @Override
            public CAMERA_FACING_ID createFromParcel(Parcel in) {
                return CAMERA_FACING_ID.values()[in.readInt()];
            }

            @Override
            public CAMERA_FACING_ID[] newArray(int size) {
                return new CAMERA_FACING_ID[size];
            }
        };
    }

    /**
     * 拍照结果回调
     */
    public interface PictureCallback {

        /**
         * 拍照数据回调
         */
        void onPictureTaken(MediaFrame frame);

        /**
         * 拍照失败信息回调
         */
        void onTakenFail(Exception e);
    }
}
