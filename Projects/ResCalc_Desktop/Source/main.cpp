// finally our code
#include "Application.h"

int main(int argc, char** argv)
{
    Cosmos::ApplicationBase::CreateInfo ci = {};
    ci.appName = "ResCalc";
    ci.customViewport = false;
    ci.validations = false;
    ci.fullscreen = false;
    ci.vsync = true;
    ci.width = 500;
    ci.height = 900;
    ci.assetsPath = "assets";

    Cosmos::Application app(ci);
    app.Run();

    return 0;
}