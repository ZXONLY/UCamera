package com.zxx.unique.camera;

public abstract class UqCameraBase {
    private static final String TAG = "UqCameraBase";

    public static final int CAMERA_TYPE_1 = 1;
    public static final int CAMERA_TYPE_2 = 2;

    public static final int CAMERA_FACING_BACK = 0;
    public static final int CAMERA_FACING_FRONT = 1;

    protected boolean mIsRunning = false;




    //-------------------------------------InUQface------------------------------------------------

    // CameraEvents are fired on the camera thread.
    public interface CameraEvents {
        /**
         * Camera open callback
         *
         */
        void onCameraOpened(int cameraType, int ret, UqCameraBase camera);

        /**
         * Camera error callback
         *
         * @param cameraType Camera type, reference
         * @param ret        Error code, reference
         * @param error      Extened error message.
         */
        void onCameraError(int cameraType, int ret, String error);

        void onCameraClosed(int cameraType, UqCameraBase camera);

        /**
         * @param infoType Reference
         * @param ext      Extended info
         * @param msg      Extended message
         */
        void onCameraInfo(int infoType, int ext, String msg);

        void onPreviewError(int cameraType, int ret, String error);

        void onPreviewSuccess(int cameraType, int infoType, int ext, String msg);

        //for camera hook
        void onPreviewStopped(int cameraType, int infoType, int ext, String msg);

    }

}
