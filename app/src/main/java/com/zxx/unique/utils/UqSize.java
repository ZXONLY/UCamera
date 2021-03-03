package com.zxx.unique.utils;

import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.Keep;

@Keep
public class UqSize implements Parcelable {
    public int width = 720;
    public int height = 1280;

    /**
     *
     * @param width width
     * @param height height
     */
    public UqSize(int width, int height) {
        this.width = width;
        this.height = height;
    }

    protected UqSize(Parcel in) {
        width = in.readInt();
        height = in.readInt();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(width);
        dest.writeInt(height);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<UqSize> CREATOR = new Creator<UqSize>() {
        @Override
        public UqSize createFromParcel(Parcel in) {
            return new UqSize(in);
        }

        @Override
        public UqSize[] newArray(int size) {
            return new UqSize[size];
        }
    };

    @Override
    public boolean equals(Object obj) {
        if(obj instanceof UqSize) {
            return width == ((UqSize)obj).width
                    && height == ((UqSize)obj).height;
        }
        return false;
    }

    public boolean isValid() {
        return width >0 && height > 0;
    }

    @Override
    public String toString() {
        return width + "*" + height;
    }
}

