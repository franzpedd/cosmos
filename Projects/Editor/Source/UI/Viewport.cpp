#include "Viewport.h"

namespace Cosmos
{
	Viewport::Viewport(ApplicationBase* app)
		: WidgetBase("Viewport", true), mApp(app)
	{
		mGrid.grid = evk_grid_create();
	}

	Viewport::~Viewport()
	{
		evk_grid_destroy(mGrid.grid);
	}

	void Viewport::Update()
	{
		// viewport
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImVec2 topPivotPos = ImGui::GetCursorScreenPos();
		evkViewportRenderphase* phase = (evkViewportRenderphase*)evk_get_renderphase(evk_Renderphase_Type_Viewport);
		ImGui::Image((ImTextureID)phase->descriptorSet, ImGui::GetContentRegionAvail());
		ImVec2 botPivotPos = ImGui::GetCursorScreenPos();
		
		DrawTopMenu(ImVec2{topPivotPos.x + 3.0f, topPivotPos.y + 3.0f});
		DrawBottomMenu(ImVec2{ botPivotPos.x + ImGui::GetContentRegionAvail().x - 90.0f, botPivotPos.y - 25.0f });

		ImGui::End();
	}

	void Viewport::Render()
	{
		if (mGrid.visible) {
            evk_grid_render(mGrid.grid);
        }
	}

	void Viewport::DrawTopMenu(const ImVec2& pivotPos)
	{
		constexpr ImVec4 activeColor = { 1.0f, 1.0f, 1.0f, 0.5f };
		ImGui::SetNextWindowPos(pivotPos);

		ImGui::BeginChild("##ViewportTopMenu");
		{
			Grid::DrawSnappingValue(&mGrid.snappingValue);
			ImGui::SameLine();
			Grid::DrawEnableSnapping(&mGrid.snapping, activeColor);
			ImGui::SameLine();
			Grid::DrawVisibility(&mGrid.visible, activeColor);
		}
		ImGui::EndChild();
	}

	void Viewport::DrawBottomMenu(const ImVec2& pivotPos)
	{
		ImGui::SetNextWindowPos(pivotPos);

		ImGui::BeginChild("##ViewportBotMenu");
		{
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 0.5f), "%.2f FPS", mApp->GetAverageFPS());
		}
		ImGui::EndChild();
	}

	void Viewport::Grid::DrawSnappingValue(float* snappingValue)
	{
		if (!snappingValue) return;

		ImGui::PushItemWidth(50.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarPadding, 2.0f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);

		ImGui::SliderFloat("##Snapping", snappingValue, 0.005f, 10.0f, "%.2f");

		ImGui::PopStyleVar();
		ImGui::PopItemWidth();

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Grid snapping value"); }
	}

	void Viewport::Grid::DrawEnableSnapping(bool* snapping, const ImVec4& activeColor)
	{
		if (!snapping) return;

		bool selected = *(snapping);
        if (selected) { ImGui::PushStyleColor(ImGuiCol_Button, activeColor); }

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);

        if (ImGui::Button(ICON_LC_MAGNET)) { *(snapping) = !*(snapping); }
        if (selected) { ImGui::PopStyleColor(); }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Enables/Disables snapping with the grid"); }
	}

	void Viewport::Grid::DrawVisibility(bool* visible, const ImVec4& activeColor)
	{
		if (!visible) return;

		bool selected = *(visible);
        if (selected) { ImGui::PushStyleColor(ImGuiCol_Button, activeColor); }

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5.0f);

        if (ImGui::Button(ICON_LC_GRID_3X3)) { *(visible) = !*(visible); }

        if (selected) { ImGui::PopStyleColor(); }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { ImGui::SetTooltip("Enables/Disables grid on viewport"); }
	}
}