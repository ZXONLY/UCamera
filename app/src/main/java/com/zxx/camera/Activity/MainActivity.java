package com.zxx.camera.Activity;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.view.View;
import android.widget.Toast;

import com.zxx.camera.R;
import com.zxx.camera.View.SurfaceCamera2Activity;
//TODO：实现Camera1，将两者整合在一起？
public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        checkPermission();
    }

    public void startCameraActivity(View view) {
        Intent intent = null;
        switch (view.getId()) {
            case R.id.camera_btn1:
                intent = new Intent(this, SurfaceCamera2Activity.class);
                break;
            case R.id.camera_btn2:
                intent = new Intent(this, TriangleActivity.class);
                break;
            case R.id.camera_btn3:
                intent = new Intent(this, GLSurfaceActivity.class);
                break;
            case R.id.camera_btn4:
                intent = new Intent(this, EGLCameraActivity.class);
                break;
            case R.id.camera_btn5:
                intent = new Intent(this, newEGLCamera2Activity.class);
                break;
            case R.id.camera_btn6:
                intent = new Intent(this, NativeCameraActivity.class);
                break;
            case R.id.camera_btn7:
                intent = new Intent(this, DecodeActivity.class);
                break;
        }
        startActivity(intent);
    }

    private void checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            String[] permissions = new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO};
            for (String permission : permissions) {
                if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(this, permissions, 200);
                    return;
                }
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,String[] permissions,  int[]
            grantResults) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && requestCode == 200) {
            for (int i = 0; i < permissions.length; i++) {
                if (grantResults[i] != PackageManager.PERMISSION_GRANTED) {
                    Toast.makeText(this, "请在设置中打开摄像头和存储权限", Toast.LENGTH_SHORT).show();
                    Intent intent = new Intent();
                    intent.setAction(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
                    Uri uri = Uri.fromParts("package", getPackageName(), null);
                    intent.setData(uri);
                    startActivityForResult(intent, 200);
                    return;
                }
            }
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK && requestCode == 200) {
            checkPermission();
        }
    }

}