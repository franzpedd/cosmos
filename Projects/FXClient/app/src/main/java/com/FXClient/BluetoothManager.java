package com.FXClient;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import androidx.annotation.RequiresPermission;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class BluetoothManager
{
    /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Native/C++ methods
    /// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// @brief called when a bluetooth device is discovered during scanning
    public native void OnDeviceFound(String address, String name);

    /// @brief called when the status of a connection changes
    public native void OnConnectionChanged(boolean connected);

    /// @brief called when data is received from the connected device
    public native void OnDataReceived(byte[] data);

    /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Java Methods
    /// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// @brief constructor
    public BluetoothManager()
    {
        mAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    /// @brief initializes the bluetooth manager, returns false if not supported
    public boolean Initialize()
    {
        if(mAdapter == null)
        {
            Log.e(mTAG, "Bluetooth not supported on this Device");
            return false;
        }
        return true;
    }

    /// @brief checks if bluetooth manager is enabled or not
    public boolean IsEnabled()
    {
        return mAdapter != null && mAdapter.isEnabled();
    }


    /// @brief starts scanning for bluetooth devices
    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    public void StartDiscover()
    {
        if(!IsEnabled())
        {
            Log.e(mTAG, "Cannot discover since Bluetooth is not enabled");
            return;
        }

        mAdapter.startDiscovery();
        Log.d(mTAG, "Discovery started");
    }

    /// @brief a bluetooth device was found
    @RequiresPermission(Manifest.permission.BLUETOOTH_CONNECT)
    public void DeviceDiscovered(BluetoothDevice device)
    {
        String name = device.getName();
        String address = device.getAddress();

        Log.d(mTAG, "Device discovered: " + name + " [" + address + "]");
        OnDeviceFound(address, name); // send to cpp
    }

    /// @brief connects to a bluetooth device
    @RequiresPermission(Manifest.permission.BLUETOOTH_SCAN)
    public boolean ConnectToDevice(String address)
    {
        try
        {
            Log.d(mTAG, "Attempting to connect to: " + address);

            BluetoothDevice device = mAdapter.getRemoteDevice(address);
            mAdapter.cancelDiscovery();
            mSocket = device.createRfcommSocketToServiceRecord(mSPPUUID);
            mSocket.connect();

            mInputStream = mSocket.getInputStream();
            mOutputStream = mSocket.getOutputStream();

            mConnected = true;
            Log.d(mTAG, "Successfully connected to: " + address);

            OnConnectionChanged(true); // send to cpp
            _StartRecording();

            return true;
        } catch (IOException e)
        {
            Log.e(mTAG, "Connection failed: " + e.getMessage());
            mConnected = false;
            OnConnectionChanged(false); // send to cpp
            return false;
        }
    }

    /// @brief starts a background thread that continuously reads data from the connected device, runs until connection closes
    private void _StartRecording()
    {
        Thread readingThread = new Thread(() -> {
            byte[] buffer = new byte[1024]; // buffer for incoming data
            Log.d(mTAG, "Reading thread started");

            while(mConnected)
            {
                try
                {
                    // blocking read, waits for data to arrive
                    int bytesRead = mInputStream.read(buffer);

                    if(bytesRead > 0)
                    {
                        // copy only the bytes that were actually read
                        byte[] data = new byte[bytesRead];
                        System.arraycopy(buffer, 0, data, 0, bytesRead);
                        Log.d(mTAG, "Received " + bytesRead + " bytes");
                        OnDataReceived(data); // send to cpp
                    }
                } catch (IOException e)
                {
                    Log.e(mTAG, "Read error: " + e.getMessage());
                    _Disconnect();
                    break;
                }
            }
        });
        readingThread.start();
    }

    /// @brief disconnects from the current device and clean up resources
    private void _Disconnect()
    {
        Log.d(mTAG, "Disconnecting bluetooth...");
        mConnected = false;

        try
        {
            if(mInputStream != null)
            {
                mInputStream.close();
                mInputStream = null;
            }

            if(mOutputStream != null)
            {
                mOutputStream.close();
                mOutputStream = null;
            }

            if(mSocket != null)
            {
                mSocket.close();
                mSocket = null;
            }
        } catch (IOException e)
        {
            Log.e(mTAG, "Disconnect error: " + e.getMessage());
        }

        Log.i(mTAG, "Disconnected");
        OnConnectionChanged(false); // send to cpp
    }

    /// @brief send data to connected device
    private boolean SendData(byte[] data)
    {
        if(!mConnected || mOutputStream == null)
        {
            Log.e(mTAG, "Cannot send data because: not connected");
            return false;
        }

        try
        {
            Log.d(mTAG, "Sending " + data.length + " bytes");
            mOutputStream.write(data);
            mOutputStream.flush(); // send immediately
            return true;
        } catch (IOException e)
        {
            Log.e(mTAG, "Send error: " + e.getMessage());
            return false;
        }
    }

    /// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Members
    /// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private final BluetoothAdapter mAdapter;
    private BluetoothSocket mSocket;
    private InputStream mInputStream;
    private OutputStream mOutputStream;
    private boolean mConnected = false;
    private static final String mTAG = "FXClient";
    private static final UUID mSPPUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); // used by most bluetooth devices
}
