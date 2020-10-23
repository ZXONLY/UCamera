package com.zxx.camera.Camera;

import android.content.Context;
import android.media.ImageReader;
import android.util.SparseIntArray;
import android.view.Display;
import android.view.OrientationEventListener;
import android.view.Surface;

public class ScreenOrientationDetector {
    private static final SparseIntArray DISPLAY_ORIENTATIONS = new SparseIntArray();

    static {
        DISPLAY_ORIENTATIONS.put(Surface.ROTATION_0,0);
        DISPLAY_ORIENTATIONS.put(Surface.ROTATION_90,90);
        DISPLAY_ORIENTATIONS.put(Surface.ROTATION_180,180);
        DISPLAY_ORIENTATIONS.put(Surface.ROTATION_270,270);
    }
    private int mLastRotation = 0;
    private Display mDisplay;

    private final OrientationEventListener mOrientationEventListener;
    private OnDisplayChangedListener mListener;

    public ScreenOrientationDetector(Context context,final OnDisplayChangedListener listener){
        this.mListener = listener;
        this.mOrientationEventListener = new OrientationEventListener(context) {
            @Override
            public void onOrientationChanged(int orientation) {
                    if(orientation==OrientationEventListener.ORIENTATION_UNKNOWN||mDisplay==null){
                        return;
                    }
                    int rotation = mDisplay.getRotation();
                    if(mLastRotation!=rotation){
                        mLastRotation = rotation;
                        mListener.onDisplayOrientationChanged(DISPLAY_ORIENTATIONS.get(mLastRotation));
                    }
            }
        };
    }

    public void enable(Display display){
        mDisplay = display;
        mOrientationEventListener.enable();
        mLastRotation = mDisplay.getRotation();
        mListener.onDisplayOrientationChanged(DISPLAY_ORIENTATIONS.get(mLastRotation));
    }

    public void disable() {
        mOrientationEventListener.disable();
        mDisplay = null;
    }


    public int getOrientation(Integer SensorOrientation){
        if(SensorOrientation == null)
            SensorOrientation = 0;
        int mLastOrientation = DISPLAY_ORIENTATIONS.get(mLastRotation);
        return (mLastOrientation + SensorOrientation +270)%360;
    }

    public boolean isLandscape() {
        int screenOrientationDegrees = DISPLAY_ORIENTATIONS.get(mLastRotation);
        return (screenOrientationDegrees == 90
                || screenOrientationDegrees == 180);
    }

    public interface OnDisplayChangedListener {

        /**
         * Called when display orientation is changed.
         *
         * @param displayOrientation One of 0, 90, 180, and 270.
         */
        void onDisplayOrientationChanged(int displayOrientation);
    }
}
