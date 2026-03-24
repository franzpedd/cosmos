#if defined(__ANDROID__)
// this in in here so SDL may do it's macro-magic and redefine int main to whatever android want's it to be
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// this is in here to initialize some JNI functions and the assets manager
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif // __ANDROID__

// finally our code
#include "Application.h"

int main(int argc, char** argv)
{
    Cosmos::ApplicationBase::CreateInfo ci = {};
    ci.appName = "ResCalc";
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
    JNIEXPORT void JNICALL Java_com_ResCalc_Main_initAssetsManager(JNIEnv* env, jclass clazz, jobject assetManager)
    {
        AAssetManager* nativeManager = AAssetManager_fromJava(env, assetManager);
    }
} // extern "c"

#endif // __ANDROID__