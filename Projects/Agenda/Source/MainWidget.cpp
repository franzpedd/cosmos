#include "MainWidget.h"

// In your header file or at the top of the cpp file
using DayClickCallback = std::function<void(int day, const char* month)>;

namespace Cosmos
{
	MainWidget::MainWidget(const ApplicationBase* app)
		: WidgetBase("Main Widget", true), mApp(app)
	{
	}

	void MainWidget::Update()
	{
        ImGui::DockSpaceOverViewport();
		DisplayMainWidget();
	}

	void MainWidget::Render()
	{
	}

    void MainWidget::DisplayMainWidget()
    {
        const ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
        uint8_t offsetMultiplier = 0;

        if (ImGui::Begin("Agenda", nullptr, flags))
        {
            if (ImGui::BeginChild("##Months", ImVec2(500, 300)))
            {
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                //DrawMonth("Jan", 31, Thursday);
                
                //ImGui::SetCursorScreenPos({ cursorPos.x + (310 * ++offsetMultiplier), cursorPos.y });
                //DrawMonth("Fev", 28, Sunday);

                //ImGui::SetCursorScreenPos({ cursorPos.x + (310 * ++offsetMultiplier), cursorPos.y });
                //DrawMonth("Mar", 31, Sunday);

                //DrawMonth("Abr", 30, Thursday);
                //DrawMonth("Mai", 31, Friday);
                //DrawMonth("Jun", 31, Friday);
            }
            ImGui::EndChild();
            ImGui::End();
        }
    }

    void MainWidget::DrawMonth(const char* month, uint8_t days, Day startDay, ImVec2 pivotPos)
    {
        const ImVec2 buttonSize = ImVec2(35, 25);

        if (pivotPos.x != 0 || pivotPos.y != 0) {
            ImGui::SetCursorScreenPos(pivotPos);
        }

        char childId[64];
        snprintf(childId, sizeof(childId), "##%s_%p", month, this);

        if (ImGui::BeginChild(childId, ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX))
        {
            ImGui::SeparatorText(month);

            ImGui::BeginDisabled();
            {
                const char* weekdays[] = { "Seg", "Ter", "Qua", "Qui", "Sex", "Sab", "Dom" };
                for (int i = 0; i < 7; i++) {
                    ImGui::Button(weekdays[i], ImVec2(buttonSize.x, 0));
                    if (i < 6) ImGui::SameLine();
                }

                ImGui::Separator();

                // draw dummies until the first day of the month
                for (int i = 0; i < startDay; i++) {
                    ImGui::Dummy(buttonSize);
                    ImGui::SameLine();
                }
            }
            ImGui::EndDisabled();

            // draw days in a proper grid
            for (uint8_t i = 1; i <= days; i++) {
                char buff[4];
                snprintf(buff, sizeof(buff), "%2u", i);

                if ((i + startDay - 1) % 7 != 0 && i > 1) {
                    ImGui::SameLine();
                }

                if (ImGui::Button(buff, buttonSize)) {

                }
            }
        }
        ImGui::EndChild();
    }
}