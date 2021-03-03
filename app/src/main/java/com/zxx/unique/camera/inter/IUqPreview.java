package com.zxx.unique.camera.inter;

import com.zxx.unique.camera.UqCameraSettings;
import com.zxx.unique.utils.UqSize;

public interface IUqPreview {

    int start();

    UqSize getPreviewSize();

    UqCameraSettings getCameraSettings();
}
