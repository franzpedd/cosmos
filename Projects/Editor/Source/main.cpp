#include "Core/Editor.h"
#include <iostream>

int main(int argc, char** argv)
{
	Cosmos::ApplicationBase::CreateInfo ci = { 0 };
	ci.appName = "Cosmos Editor";
	ci.customViewport = true;
	ci.validations = true;
	ci.fullscreen = false;
	ci.vsync = false;
	ci.width = 1366;
	ci.height = 768;
	#if defined(_WIN32) || defined(_WIN64)
	ci.assetsPath = "../data";
	#else
	ci.assetsPath = "data";
	#endif

	Cosmos::Editor editor(ci);
	editor.Run();

	std::cout << "Hello World\n";
	return 0;
}