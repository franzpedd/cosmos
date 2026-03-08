package com.FXClient;

import android.Manifest;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.widget.Toast;
import androidx.annotation.NonNull;
import androidx.annotation.RequiresPermission;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

/*
* MainActivity handles Android lifecycle and permissions. It bridges the gap between Android's permission system and our native code.
* */
public class MainActivity extends AppCompatActivity
{
    /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Native/C++ methods
    /// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static { System.loadLibrary("native-lib"); }

    /// @brief called when Bluetooth is ready to use
    public native void OnBluetoothReady();

    /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Java Methods
    /// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// @brief called on class creation
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        mBluetoothManager = new BluetoothManager();

        if(!mBluetoothManager.Initialize())
        {
            Toast.makeText(this, "Bluetooth not supported", Toast.LENGTH_LONG).show();
            finish(); // close app if bluetooth is not supported for now
            return;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            _CheckPermissions();
        }
    }

    /// @brief called on class destruction
    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        try
        {
            unregisterReceiver(mDiscoveryReceiver);
        } catch (Exception e)
        {
            // receiver might not be registered, ignore
        }
    }

    /// @brief called when user responds to permission request dialog
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults)
    {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if(requestCode == REQUEST_BLUETOOTH_PERMISSIONS)
        {
            boolean allGranted = true;
            for(int result : grantResults)
            {
                if(result != PackageManager.PERMISSION_GRANTED)
                {
                    allGranted = false;
                    break;
                }
            }

            if(allGranted)
            {
                _StartBluetooth();
            }

            else
            {
                Toast.makeText(this, "Bluetooth permissions required", Toast.LENGTH_LONG).show();
                finish(); // close app for now
            }
        }
    }

    /// @brief checks if user has all permissions required for bluetooth usage
    private void _CheckPermissions()
    {
        // BLUETOOTH_CONNECT: to connect to devices
        // BLUETOOTH_SCAN: to scan for devices
        // ACCESS_FINE_LOCATION: still required for Bluetooth scanning (this is for old devices)
        if(ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_DENIED
        || ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED
        || ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED)
        {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                ActivityCompat.requestPermissions(this,
                        new String[]{
                                Manifest.permission.BLUETOOTH_CONNECT,
                                Manifest.permission.BLUETOOTH_SCAN,
                                Manifest.permission.ACCESS_FINE_LOCATION
                        }, REQUEST_BLUETOOTH_PERMISSIONS);
            }
        }

        else
        {
            _StartBluetooth();
        }
    }

    /// @brief start Bluetooth operations after permissions are granted
    private void _StartBluetooth()
    {
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        registerReceiver(mDiscoveryReceiver, filter);
        OnBluetoothReady();
    }

    /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Members
    /// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private BluetoothManager mBluetoothManager;
    private static final int REQUEST_BLUETOOTH_PERMISSIONS = 1;

    /// @brief listens for Bluetooth discovery events, when Android finds a device, it broadcasts an ACTION_FOUND intent.
    private final BroadcastReceiver mDiscoveryReceiver = new BroadcastReceiver() {
        @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
        @Override
        public void onReceive(Context context, Intent intent)
        {
            String action = intent.getAction();

            if(BluetoothDevice.ACTION_FOUND.equals(action)) // check if this is a device discovery event
            {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if(device != null && mBluetoothManager != null)
                {
                    mBluetoothManager.DeviceDiscovered(device);
                }
            }
        }
    };

}
