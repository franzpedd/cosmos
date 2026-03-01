#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////// android bluetooth code

#if defined(__ANDROID__)
#include <jni.h>

namespace Cosmos
{
    class BluetoothBridge
    {
    public:

        /// @brief returns the bridge singleton
        static BluetoothBridge& GetInstance();

    public:

        /// @brief initializes the bridge
        void Initialize(JNIEnv* environment, jobject bluetoothManager);

    public: // operations

        bool IsEnabled();
    };
}

#endif // __ANDROID__