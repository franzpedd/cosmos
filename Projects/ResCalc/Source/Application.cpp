#include "Application.h"
#include "ResCalcWidget.h"

namespace Cosmos
{
    Application::Application(const CreateInfo& ci)
        : ApplicationBase(ci)
    {
        mResistorCalcWidget = new ResistorCalcWidget(this);
        GetGUI()->AddWidget(mResistorCalcWidget);

        GetGUI()->SetDarkishTheme();
    }
}