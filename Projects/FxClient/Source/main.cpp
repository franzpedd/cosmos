#include <iostream>
#include "Application.h"

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