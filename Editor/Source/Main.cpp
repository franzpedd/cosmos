#include <iostream>

#include <Engine.h>

using namespace Cosmos;

int main(int argc, char* argv[])
{
    LOG_TRACE("Trace");
    LOG_INFO("Information");
    LOG_WARN("Warning");
    LOG_ERROR("Error");
    LOG_TO_FILE(Logger::Severity::Error, "error.log", "Testing file message");
    Logger() << "Testing simple Logger";

    Application* app = new Application();
    app->Run();
    delete app;

    return 0;
}

