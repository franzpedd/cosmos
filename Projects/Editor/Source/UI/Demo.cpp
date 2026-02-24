#include "Demo.h"

namespace Cosmos
{
    Demo::Demo(ApplicationBase* app)
        : WidgetBase("Demo Window", true), mApp(app)
    {
    }

    Demo::~Demo()
    {
    }

    void Demo::Update()
    {
        ImGui::ShowDemoWindow(&mVisible);

        ImGui::Begin("Stats", nullptr);

        ImGui::Text("%f fps | %f ts", mApp->GetAverageFPS(), mApp->GetTimestep());

        ImGui::End();
    }

    void Demo::Render()
    {
    }
}