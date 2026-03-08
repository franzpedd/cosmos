#include "Bluetooth.h"

///////////////////////////////////////////////////////////////////////////////////////////////// android bluetooth code

#if defined(__ANDROID__)
#include <android/log.h>

/// @brief log macros
#define LOG_TAG "BluetoothBridge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)


/// @brief global JVM reference, needed to attach threads for callbacks
static JavaVM* g_JVM = nullptr;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    g_JVM = vm;
    LOGI("JNI_OnLoad called, JavaVM stored");
    return JNI_VERSION_1_6;
}

/// @brief helper function to return the JNIEnv for the current thread
static bool JNI_GetEnv(JNIEnv** env)
{
    bool attached = false;
    jint result = g_JVM->GetEnv((void**)env, JNI_VERSION_1_6);

    if (result == JNI_EDETACHED)
    {
        // thread is not attached, attach it now
        if (g_JVM->AttachCurrentThread(env, nullptr) == JNI_OK)
        {
            attached = true;
            LOGD("Thread attached to JVM");
        }

        else
        {
            LOGE("Failed to attach thread to JVM");
            return false;
        }
    }

    else if (result != JNI_OK)
    {
        LOGE("Failed to get JNIEnv");
        return false;
    }
    return attached;
}

namespace Cosmos
{
    BluetoothBridge &BluetoothBridge::GetInstance()
    {
        static BluetoothBridge instance;
        return instance;
    }

    void BluetoothBridge::Initialize(JNIEnv* env, jobject bluetoothManager)
    {
        LOGI("Initializing BluetoothBridge");

        // store global references, preventing the object from being garbage collected
        mBluetoothManagerOjb = env->NewGlobalRef(bluetoothManager);
        jclass localClass = env->GetObjectClass(bluetoothManager);
        mBluetoothManagerClass = (jclass)env->NewGlobalRef(localClass);

        // cache methods for performance
        mIsEnabledMethod = env->GetMethodID(mBluetoothManagerClass, "IsEnabled", "()Z");
        mStartDiscoveryMethod = env->GetMethodID(mBluetoothManagerClass, "StartDiscovery", "()V");
        mConnectMethod = env->GetMethodID(mBluetoothManagerClass, "ConnectToDevice", "(Ljava/lang/String;)Z");
        mSendDataMethod = env->GetMethodID(mBluetoothManagerClass, "SendData", "([B)Z");
        mDisconnectMethod = env->GetMethodID(mBluetoothManagerClass, "Disconnect", "()V");

        // clean up local reference
        env->DeleteLocalRef(localClass);
        LOGI("BluetoothBridge initialized successfully");
    }

    bool BluetoothBridge::IsEnabled()
    {
        JNIEnv* env = nullptr;
        bool attached = JNI_GetEnv(&env);

        if (!env) {
            LOGE("isEnabled: Failed to get JNIEnv");
            return false;
        }

        jboolean result = env->CallBooleanMethod(mBluetoothManagerOjb, mIsEnabledMethod);

        if (attached)
        {
            g_JVM->DetachCurrentThread();
        }

        return result;
    }

    void BluetoothBridge::StartDiscovery()
    {
        JNIEnv* env = nullptr;
        bool attached = JNI_GetEnv(&env);

        if (!env)
        {
            LOGE("StartDiscovery: Failed to get JNIEnv");
            return;
        }

        env->CallVoidMethod(mBluetoothManagerOjb, mStartDiscoveryMethod);

        if (attached)
        {
            g_JVM->DetachCurrentThread();
        }

        LOGD("StartDiscovery called");
    }

    bool BluetoothBridge::Connect(const std::string& address)
    {
        JNIEnv* env = nullptr;
        bool attached = JNI_GetEnv(&env);

        if (!env)
        {
            LOGE("Connect: Failed to get JNIEnv");
            return false;
        }

        jstring jAddress = env->NewStringUTF(address.c_str());
        jboolean result = env->CallBooleanMethod(mBluetoothManagerOjb, mConnectMethod, jAddress);
        env->DeleteLocalRef(jAddress);

        if (attached)
        {
            g_JVM->DetachCurrentThread();
        }

        LOGD("Connect(%s) returned %d", address.c_str(), result);
        return result;
    }

    void BluetoothBridge::Disconnect()
    {
        JNIEnv* env = nullptr;
        bool attached = JNI_GetEnv(&env);

        if (!env)
        {
            LOGE("Disconnect: Failed to get JNIEnv");
            return;
        }

        env->CallVoidMethod(mBluetoothManagerOjb, mDisconnectMethod);

        if (attached)
        {
            g_JVM->DetachCurrentThread();
        }

        LOGD("Disconnect called");
    }

    bool BluetoothBridge::SendData(const std::vector<uint8_t>& data)
    {
        JNIEnv* env = nullptr;
        bool attached = JNI_GetEnv(&env);

        if (!env)
        {
            LOGE("SendData: Failed to get JNIEnv");
            return false;
        }

        jbyteArray jData = env->NewByteArray(data.size());
        env->SetByteArrayRegion(jData, 0, data.size(), reinterpret_cast<const jbyte*>(data.data()));
        jboolean result = env->CallBooleanMethod(mBluetoothManagerOjb, mSendDataMethod, jData);
        env->DeleteLocalRef(jData);

        if (attached)
        {
            g_JVM->DetachCurrentThread();
        }

        LOGD("SendData(%zu bytes) returned %d", data.size(), result);
        return result;
    }

    void BluetoothBridge::OnDeviceFound(JNIEnv *env, jstring address, jstring name) const
    {
        const char* addressStr = env->GetStringUTFChars(address, nullptr);
        const char* nameStr = env->GetStringUTFChars(name, nullptr);

        LOGD("OnDeviceFound: %s [%s]", nameStr, addressStr);

        if (OnDeviceFoundCallback) { OnDeviceFoundCallback(addressStr, nameStr); }

        env->ReleaseStringUTFChars(address, addressStr);
        env->ReleaseStringUTFChars(name, nameStr);
    }

    void BluetoothBridge::OnConnectionChanged(JNIEnv *env, jboolean connected) const
    {
        bool isConnected = (connected == JNI_TRUE);
        LOGD("OnConnectionChanged: %s", isConnected ? "connected" : "disconnected");

        if (OnConnectionChangedCallback) { OnConnectionChangedCallback(isConnected); }
    }

    void BluetoothBridge::OnDataReceived(JNIEnv *env, jbyteArray data) const
    {
        jsize length = env->GetArrayLength(data);
        jbyte* elements = env->GetByteArrayElements(data, nullptr);
        std::vector<uint8_t> receivedData(elements, elements + length);

        LOGD("OnDataReceived: %zu bytes", receivedData.size());

        if (OnDataReceivedCallback) { OnDataReceivedCallback(receivedData); }

        env->ReleaseByteArrayElements(data, elements, JNI_ABORT);
    }
}

extern "C"
{
    /// @brief called from java when a device is discovered
    JNIEXPORT void JNICALL Java_com_FXClient_BluetoothManager_OnDeviceFound(JNIEnv* env, jobject thiz, jstring address, jstring name)
    {
        Cosmos::BluetoothBridge::GetInstance().OnDeviceFound(env, address, name);
    }

    /// @brief called from Java when connection state changes
    JNIEXPORT void JNICALL Java_com_FXClient_BluetoothManager_OnConnectionChanged(JNIEnv* env, jobject thiz, jboolean connected)
    {
        Cosmos::BluetoothBridge::GetInstance().OnConnectionChanged(env, connected);
    }

    /// @brief called from Java when data is received
    JNIEXPORT void JNICALL Java_com_FXClient_BluetoothManager_OnDataReceived(JNIEnv* env, jobject thiz, jbyteArray data)
    {
        Cosmos::BluetoothBridge::GetInstance().OnDataReceived(env, data);
    }
} // extern "C"

#endif // __ANDROID__