// this in in here so SDL may do it's macro-magic and redefine int main to whatever android want's it to be
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// this is in here to initialize some JNI functions and the assets manager
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

// finally our code
#include "ResCalc/Application.h"

int main(int argc, char** argv)
{
    Cosmos::ApplicationBase::CreateInfo ci = {};
    ci.appName = "ResCalc";
    ci.customViewport = false;
    ci.validations = false;
    ci.fullscreen = true;
    ci.vsync = true;
    ci.width = 1080;            // don't matter since it's fullscreen
    ci.height = 2040;           // don't matter since it's fullscreen
    ci.assetsPath = "assets";   // this is always the case on android

    Cosmos::Application app(ci);
    app.Run();

    return 0;
}

/// @brief called from java to setup the assets manager
extern "C" JNIEXPORT void JNICALL Java_com_ResCalc_Main_initAssetsManager(JNIEnv* env, jclass clazz, jobject assetManager) {
    AAssetManager* nativeManager = AAssetManager_fromJava(env, assetManager);
    //cren_android_assets_manager_init(nativeManager);
}