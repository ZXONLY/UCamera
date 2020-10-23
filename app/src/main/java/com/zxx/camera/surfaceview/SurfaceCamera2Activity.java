package com.zxx.camera.surfaceview;

import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.Image;
import android.media.ImageReader;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.os.EnvironmentCompat;

import com.zxx.camera.Camera.camera2Proxy;
import com.zxx.camera.R;
import com.zxx.camera.Utils.ImageUtils;
import com.zxx.camera.glSurfaceview.Camera2GLSurfaceView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.Date;

public class SurfaceCamera2Activity extends AppCompatActivity implements View.OnClickListener {

    private static final String TAG = "SurfaceCameraActivity";

    private ImageView mCloseIv;
    private ImageView mSwitchCameraIv;
    private ImageView mTakePictureIv;
    private ImageView mPictureIv;
    private Camera2SurfaceView mCameraView;
    private camera2Proxy mCameraProxy;

    private static final String GALLERY_PATH = Environment.getExternalStoragePublicDirectory(Environment
            .DIRECTORY_DCIM) + File.separator + "Camera";
    private static final SimpleDateFormat DATE_FORMAT = new SimpleDateFormat("yyyyMMdd_HHmmss");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_glsurfacecamera2);
        mCloseIv = findViewById(R.id.toolbar_close_iv);
        mCloseIv.setOnClickListener(this);
        mSwitchCameraIv = findViewById(R.id.toolbar_switch_iv);
        mSwitchCameraIv.setOnClickListener(this);
        //mTakePictureIv = findViewById(R.id.toolbar_switch_iv);
        mPictureIv = findViewById(R.id.picture_iv);
                mPictureIv.setImageBitmap(getLatestThumbBitmap());
        mTakePictureIv = findViewById(R.id.take_picture_iv);
        mTakePictureIv.setOnClickListener(this);
        mCameraView = findViewById(R.id.camera_view);
        mCameraProxy = mCameraView.getCameraProxy();
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.toolbar_close_iv:
                finish();
                break;
            case R.id.toolbar_switch_iv:
                mCameraProxy.switchCamera(mCameraView.getWidth(), mCameraView.getHeight());
                mCameraProxy.startPreview();
                break;
            case R.id.take_picture_iv:
                mCameraProxy.setImageAvailableListener(mOnImageAvailableListener);
                mCameraProxy.captureStillPicture(); // 拍照
                break;
            case R.id.picture_iv:
                Intent intent = new Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                startActivity(intent);
                break;
        }
    }


    public final ImageReader.OnImageAvailableListener mOnImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            String fileName = DATE_FORMAT.format(new Date(System.currentTimeMillis())) + ".jpg";
            File outFile = new File(GALLERY_PATH, fileName);
            Log.d(TAG, "saveImage. filepath: " + outFile.getAbsolutePath());
            FileOutputStream os = null;
            ByteBuffer buffer = reader.acquireNextImage().getPlanes()[0].getBuffer();
            byte[] bytes = new byte[buffer.remaining()];
            buffer.get(bytes);
            if (!Environment.MEDIA_MOUNTED.equals(EnvironmentCompat.getStorageState(outFile))) {
                Log.e(TAG,"no permission");
                return;
            }
            Log.i(TAG,"have permission");
            try {
                os = new FileOutputStream(outFile);
                os.write(bytes);
                os.flush();
                os.close();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                if (os != null) {
                    try {
                        os.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
            mPictureIv.setImageBitmap(getLatestThumbBitmap());
        }
    };
    private static final String[] STORE_IMAGES = {
            MediaStore.Images.Thumbnails._ID,
    };

    public Bitmap getLatestThumbBitmap() {
        Bitmap bitmap = null;
        // 按照时间顺序降序查询
        Cursor cursor = MediaStore.Images.Media.query(this.getContentResolver(), MediaStore.Images.Media
                .EXTERNAL_CONTENT_URI, STORE_IMAGES, null, null, MediaStore.Files.FileColumns.DATE_MODIFIED + " DESC");
        boolean first = cursor.moveToFirst();
        if (first) {
            long id = cursor.getLong(0);
            bitmap = MediaStore.Images.Thumbnails.getThumbnail(this.getContentResolver(), id, MediaStore.Images
                    .Thumbnails.MICRO_KIND, null);
            Log.d(TAG, "bitmap width: " + bitmap.getWidth());
            Log.d(TAG, "bitmap height: " + bitmap.getHeight());
        }
        cursor.close();
        return bitmap;
    }
}
