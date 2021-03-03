package com.zxx.unique.camera.inter;

import com.zxx.unique.camera.UqCameraSettings;
import com.zxx.unique.utils.UqSize;

public interface IUqCamera extends IUqPreview{
    int open();

    int switchCameraMode(int mode);

    int switchCameraMode(int mode ,UqCameraSettings settings);

    int startPreview();

    int close();

    //int close(boolean sync);

    void destroy();

    int switchCamera();

    int switchCamera(UqCameraSettings.CAMERA_FACING_ID cameraFacing);

    UqCameraSettings.CAMERA_FACING_ID getCameraFacing();

    int takePicture(UqCameraSettings.PictureCallback callback);

    int focusAtPoint(final int viewWidth, final int viewHeight, final float displayDensity, final int x, final int y);

    //int focusAtPoint(FocusSettings FocusSettings);

    void cancelFocus();

    /**
     * enable continue-auto-focus if support
     */
    void enableCaf();

    /**
     * Get max zoom value that camera support
     *
     * @return Real value when success and negative number failed.
     */
    int queryZoomAbility();

    int queryZoomAbility(boolean isZoomV2);

    float queryShaderZoomAbility();

    /**
     * Start zoom with specific value.
     *
     */
    int startZoom(float zoom);

    /**
     * Stop zoom. Call this when camera support smooth zoom.
     *
     */
    int stopZoom();

    int getCameraState();

    UqSize getPreviewFramsSize();

}
