#include "Application.h"

namespace Cosmos
{
    Application::Application(const CreateInfo& ci)
        : ApplicationBase(ci)
    {
        mCoreWidget = new CoreWidget(this);
        GetGUI()->AddWidget(mCoreWidget);
    }

    Application::~Application()
    {
        delete mCoreWidget;
    }
}