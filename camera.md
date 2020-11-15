## 一.Camera2介绍

Camera2 是谷歌在Android5.0以后新增的接口，采用了新的机制，新的类进行拍照和录制。Camera2相较于Camera1增添了新的特性和功能，如

*  支持30fps全高清连拍

* 支持RAW格式的照片拍摄

Camera2是基于Pipeline，Client(也就是app)需要向相机Service发送请求，等待Service的响应。

## 二.Camera的大致拍摄和预览流程

* 从CameraService中获取到CameraManager

  ```java
  mCameraManager = (CameraManager) mActivity.getSystemService(Context.CAMERA_SERVICE);
  ```

* 从CameraManager中获取到CameraCharacteristics, 配置预览页面,然后打开相机(需要设置CameraDevice.StateCallback)

  ```java
  mCameraCharacteristics = mCameraManager.getCameraCharacteristics(Integer.toString(mCameraId));
              StreamConfigurationMap streamConfigurationMap = mCameraCharacteristics.get(CameraCharacteristics
                      .SCALER_STREAM_CONFIGURATION_MAP);
              //预览大小
              mPreviewSize = chooseOptimalSize(streamConfigurationMap.getOutputSizes(SurfaceTexture.class),width,height);
  ```

* 创建一个从Pipeline获取图片的CaptureRequest.Builder，创建Surface，并将其其设置为Target Surface

  ```java
  mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
  mPreviewRequestBuilder.addTarget(mPerviewSurface);
  ```

* 创建CaptureSession，并在CameraCaptureSession.StateCallback回调中生成CaptureRequest,就开始预览了，CaptureSession需要两个surface，这里我们一个用于预览，另一个ImageReader用于拍照。

  ```java
  mCameraDevice.createCaptureSession(Arrays.asList(mPerviewSurface, mImageReader.getSurface()),
                      new CameraCaptureSession.StateCallback() {
                          @Override
                          public void onConfigured(@NonNull CameraCaptureSession session) {
                              mCameraCaptureSession = session;
                              // 设置连续自动对焦
                              mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest
                                      .CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                              // 设置自动曝光
                              mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest
                                      .CONTROL_AE_MODE_ON_AUTO_FLASH);
                              // 设置完后自动开始预览
                              mPreviewRequest = mPreviewRequestBuilder.build();
                              startPreview();
                          }
  
                          @Override
                          public void onConfigureFailed(@NonNull CameraCaptureSession session) {
                              Log.e(TAG, "ConfigureFailed. session: mCaptureSession");
                          }
                      }, mBackgroundHandler); // handle 传入 null 表示使用当前线程的 Looper
  ```

* 拍摄和预览过程非常类似，只要创建出和拍照对应的CaptureRequest.Builder(模版TEMPLATE_STILL_CAPTURE)，

  * 注：存储照片到相册时，系统相册可能暂时无法显示，需要重启后才会显示

## 三.Camera2的相关数据结构

### 1. CameraManager

> cameraManager是负责查询和与相机Service建立连接的功能。

* cameraManager提供了获取CameraCharacteristics实例的方法

  ``` java
  mCameraCharacteristics = mCameraManager.getCameraCharacteristics(Integer.toString(mCameraId));
  ```

* 根据指定的相机ID连接相机设备

  ``` java
  mCameraManager.openCamera(Integer.toString(mCameraId), mStateCallback, mBackgroundHandler);
  ```

### 2. CameraCharacteristics

CameraCharacteristics是封装了对应相机的基本信息，如相机的朝向``` LENS_FACING ```、闪光灯的能力、可使用的AE模式等等。

### 3. CameraDevice

CameraDevice代表的是当前连接的设备，在成功打开相机后能够获取到对应的CameraDevice。CameraDevice有如下几个功能：

* 根据指定的参数创建CameraCaptureSession
* 根据指定的模板创建Capture.builder
* 关闭相机设备
* 监听当前相机设备的状态，通过CameraDevice.StateCallback的回调监听当前相机设备打开成功，打开失败，断开连接

### 4. Surface

### 5. CameraCaptureSession

### 6. CaptureRequest

### 7. CameraResult



























​	



