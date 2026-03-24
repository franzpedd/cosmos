#include "Dockspace.h"

namespace Cosmos
{
    Dockspace::Dockspace(ApplicationBase* app)
        : WidgetBase("Dockspace Window", true), mApp(app)
    {
    }

    Dockspace::~Dockspace()
    {
    }

    void Dockspace::Update()
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        ImGui::Begin("##Dockspace", 0, ImGuiWindowFlags_NoDecoration);
        ImGui::DockSpace(ImGui::GetID("MyDockspace"));
        ImGui::End();
    }

    void Dockspace::Render()
    {
    }
}