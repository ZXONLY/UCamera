package com.zxx.camera.Camera;

public abstract class CameraClient {

    abstract void init();

    abstract void openCamera(ResManage res);

    abstract void switchCamera(ResManage res);

    abstract void captureStillPicture();

    abstract int getOrientation();

}
