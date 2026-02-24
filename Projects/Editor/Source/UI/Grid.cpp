#include "Grid.h"

namespace Cosmos
{
    Grid::Grid(ApplicationBase* app)
        : WidgetBase("Grid", true), mApp(app)
    {
        mGrid = evk_grid_create();
    }

    Grid::~Grid()
    {
        evk_grid_destroy(mGrid);
    }

    void Grid::Update()
    {
        constexpr ImVec4 activeCol = { 1.0f, 1.0f, 1.0f, 0.5f };

        mMenuPosition = mApp->GetWindow()->GetPos();
        ImGui::SetNextWindowPos({ mMenuPosition.xy.x, mMenuPosition.xy.y });

        ImGui::Begin("##ViewportMenubar", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);

        // snapping value
        {
            ImGui::PushItemWidth(50.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarPadding, 2.0f);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);

            ImGui::SliderFloat("##Snapping", &mSnappingValue, 0.005f, 10.0f, "%.2f");

            ImGui::PopStyleVar();
            ImGui::PopItemWidth();

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Grid snapping value"); }
        }

        ImGui::SameLine();
        
        // enable/disable snapping
        {
            bool selected = mSnapping;
            if (selected) { ImGui::PushStyleColor(ImGuiCol_Button, activeCol); }
        
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
        
            if (ImGui::Button(ICON_LC_MAGNET)) { mSnapping = !mSnapping; }
            if (selected) { ImGui::PopStyleColor(); }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Enables/Disables snapping with the grid"); }
        }
        
        ImGui::SameLine();
        
        // visibility
        {
            bool selected = mVisible;
            if (selected) { ImGui::PushStyleColor(ImGuiCol_Button, activeCol); }
        
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);
        
            if (ImGui::Button(ICON_LC_GRID_3X3)) { mVisible = !mVisible; }
        
            if (selected) { ImGui::PopStyleColor(); }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Enables/Disables grid on viewport"); }
        }

        ImGui::End();
    }

    void Grid::Render()
    {
        if (mVisible) {
            evk_grid_render(mGrid);
        }
    }

    void Grid::SetMenuPosition(float2 pos)
    {
        mMenuPosition = pos;
    }
}