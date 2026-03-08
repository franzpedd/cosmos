#if defined(__ANDROID__)
// this in in here so SDL may do it's macro-magic and redefine int main to whatever android want's it to be
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// this is in here to initialize some JNI functions and the assets manager
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

/// @brief log macro, really should do something about this on engine side
#define LOG_TAG "NativeLib"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#endif // __ANDROID__

// finally our code
#include "FXClient/Application.h"
#include "FXClient/Bluetooth.h"

int main(int argc, char** argv)
{
    Cosmos::Application::CreateInfo ci = {};
    ci.appName = "FXClient";
    ci.customViewport = false;
    ci.validations = true;
    ci.fullscreen = false;
    ci.vsync = true;
    ci.width = 500;
    ci.height = 900;
    ci.assetsPath = "assets";

    Cosmos::Application app(ci);
    app.Run();

    return 0;
}

#if defined(__ANDROID__)

extern "C"
{
    /// @brief called from java to setup the assets manager
    JNIEXPORT void JNICALL Java_com_FXClient_Main_initAssetsManager(JNIEnv *env, jclass clazz, jobject assetManager)
    {
        AAssetManager* nativeManager = AAssetManager_fromJava(env, assetManager);
        // init assets manager on engine's side is not applicable right now
    }

    /// @brief called from java when bluetooth is ready to use
    JNIEXPORT void JNICALL Java_com_FXClient_MainActivity_OnBluetoothReady(JNIEnv* env, jobject /* this */)
    {
        auto& bridge = Cosmos::BluetoothBridge::GetInstance();

        /// @brief callback when devices are found
        bridge.OnDeviceFoundCallback = [](const std::string& address, const std::string& name)
        {
            LOGI("Device found: %s [%s]", name.c_str(), address.c_str());

            // things I can do here:
            // display the device in a list
            // check if it's ESP32 by name or service
            // auto-connect if it's the right device

            // example
            if (name.find("ESP32") != std::string::npos) {
                LOGI("Found ESP32, connecting...");
                Cosmos::BluetoothBridge::GetInstance().Connect(address);
            }
        };

        /// @brief callback when connection status changes
        bridge.OnConnectionChangedCallback = [](bool connected)
        {
            if(connected)
            {
                LOGI("Connected to device");

                // send initial commands to ESP32
                std::vector<uint8_t> handshake = {'H', 'E', 'L', 'L', 'O'};
                Cosmos::BluetoothBridge::GetInstance().SendData(handshake);
            }

            else
            {
                LOGI("Disconnected from device");
            }
        };

        /// @brief callback when data is received from ESP32
        bridge.OnDataReceivedCallback = [](const std::vector<uint8_t>& data)
        {
            LOGI("Received %zu bytes", data.size());

            std::string text(data.begin(), data.end());
            LOGI("Data: %s", text.c_str());
        };

        if(bridge.IsEnabled())
        {
            LOGI("Bluetooth is enabled, starting discovery...");
            bridge.StartDiscovery();
        }

        else
        {
            LOGI("Bluetooth is not enabled");
            // request user to enable bluetooth?
        }
    }
} // extern "c"

#endif // __ANDROID__