#if defined(__ANDROID__)
/// @brief include SDL files to it can do its thing
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/// @brief includes JNI stuff as well
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>

/// @brief define bridge functions between Java and C/C++
extern "c"
{
	/// @brief in the case we use assets we must set the asset manager
	JNIEXPORT void JNICALL Java_com_Agenda_Main_initAssetManager(JNIEnv* env, jclass clazz, jobject assetManager)
	{
		LOG_TO_TERMINAL(Cosmos::Logger::Todo, "The correct grammar should be _initAssetManager, check other projects");
		AAssetManager* nativeManager = AAssetManager_fromJava(env, assetManager);
	}
} // extern "c"

#endif // __ANDROID__

#include "Application.h"

int main(int argc, char** argv)
{
	Cosmos::ApplicationBase::CreateInfo ci = { 0 };
	ci.appName = "Agenda";
	ci.assetsPath = "assets";
	ci.customViewport = false;
	ci.fullscreen = false;
	ci.validations = false;
	ci.vsync = true;
	ci.width = 1366;
	ci.height = 728;
	
	Cosmos::Agenda::Application app(ci);
	app.Run();

	return 0;
}