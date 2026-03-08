#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////// android bluetooth code

#if defined(__ANDROID__)
#include <jni.h>
#include <functional>
#include <string>
#include <vector>

namespace Cosmos
{
    class BluetoothBridge
    {
    public:

        /// @brief returns the bridge singleton
        static BluetoothBridge& GetInstance();

        /// @brief initializes the bridge
        void Initialize(JNIEnv* env, jobject bluetoothManager);

        /// @brief checks if bluetooth is enabled on the device
        bool IsEnabled();

        /// @brief starts scanning for nearby bluetooth devices
        void StartDiscovery();

        /// @brief connects to a specific bluetooth device, returns true on connected successfully
        bool Connect(const std::string& address);

        /// @brief disconnects from current connected device
        void Disconnect();

        /// @brief send data to the connected device, returns true if sent successfully
        bool SendData(const std::vector<uint8_t>& data);

    public:

        /// @brief called by JNI when a device was found
        void OnDeviceFound(JNIEnv* env, jstring address, jstring name) const;

        /// @brief called by JNI when the connection status changed
        void OnConnectionChanged(JNIEnv* env, jboolean connected) const;

        /// @brief called by JNI when data is received
        void OnDataReceived(JNIEnv* env, jbyteArray data) const;

    private:

        /// @brief constructor
        BluetoothBridge() = default;

        /// @brief destructor
        ~BluetoothBridge() = default;

        /// @brief delete copy constructor
        BluetoothBridge(const BluetoothBridge&) = delete;

        /// @brief delete assignment constructor
        BluetoothBridge& operator=(const BluetoothBridge&) = delete;

    private: // JNI references - all global references

        jclass mBluetoothManagerClass = nullptr;
        jobject mBluetoothManagerOjb = nullptr;
        jmethodID mIsEnabledMethod = nullptr;
        jmethodID  mStartDiscoveryMethod = nullptr;
        jmethodID mConnectMethod = nullptr;
        jmethodID mDisconnectMethod = nullptr;
        jmethodID mSendDataMethod = nullptr;

    public: // callbacks

        std::function<void(const std::string&, const std::string&)> OnDeviceFoundCallback;
        std::function<void(bool)> OnConnectionChangedCallback;
        std::function<void(const std::vector<uint8_t>&)> OnDataReceivedCallback;
    };
}

#endif // __ANDROID__