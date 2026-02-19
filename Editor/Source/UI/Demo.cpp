#include "Demo.h"

namespace Cosmos
{
    Demo::Demo()
        : WidgetBase("Demo Window", true)
    {
    }

    Demo::~Demo()
    {
    }

    void Demo::Update()
    {
        ImGui::ShowDemoWindow(&mVisible);
    }

    void Demo::Render()
    {
    }
}