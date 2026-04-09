#include "CoreWidget.h"

namespace Cosmos
{
    CoreWidget::CoreWidget(ApplicationBase* app)
            : WidgetBase("Core Widget", true), mApp(app)
    {
        mTremolo = new Tremolo("Tremolo");
        mOverdrive = new Overdrive("Overdrive");
        mDistortion = new Distortion("Distortion");
        mDelay = new Delay("Delay");
        mReverb = new Reverb("Reverb");
        mCompression = new Compression("Compression");

        mAllFX.push_back(mTremolo);
        mAllFX.push_back(mOverdrive);
        mAllFX.push_back(mDistortion);
        mAllFX.push_back(mDelay);
        mAllFX.push_back(mReverb);
        mAllFX.push_back(mCompression);
    }

    CoreWidget::~CoreWidget()
    {
        delete mTremolo;
        delete mOverdrive;
        delete mDistortion;
        delete mDelay;
        delete mReverb;
        delete mCompression;
    }

    void CoreWidget::Update()
    {
        float2 size = mApp->GetWindow()->GetSize();
        float2 pos = mApp->GetWindow()->GetPos();
        ImGui::SetNextWindowPos(ImVec2(pos.xy.x, pos.xy.y));
        ImGui::SetNextWindowSize(ImVec2(size.xy.x, size.xy.y));

        ImGui::Begin("##CoreWidgetWindow", nullptr, ImGuiWindowFlags_NoDecoration);

        DrawTopMenu();

        ImGui::End();
    }

    void CoreWidget::Render()
    {
    }

    void CoreWidget::DrawTopMenu()
    {
        // colors
        ImVec4* colors = mApp->GetGUI()->GetStyleColors();
        ImVec4 selectedColor = ImVec4(.5f, .5f, .5f, 1.0f);
        ImVec4 buttonColor = colors[ImGuiCol_Button];

        // sizes
        ImVec2 winSize = ImGui::GetContentRegionAvail();
        constexpr float offset = 5.0f;
        const float ySize = 30.0f * GUI::GetFontScalar();
        float xSize = (winSize.x / 3) - offset;

        static int32_t selectedOption = 1;

        // draw top menu
        Widget::ButtonOption("View", 0, &selectedOption, selectedColor, buttonColor, ImVec2(xSize, ySize));

        ImGui::SameLine();

        Widget::ButtonOption("Add", 1, &selectedOption, selectedColor, buttonColor, ImVec2(xSize, ySize));

        ImGui::SameLine();

        Widget::ButtonOption("Settings", 2, &selectedOption, selectedColor, buttonColor, ImVec2(xSize, ySize));

        // draw canvas
        switch (selectedOption)
        {
            case 0: { DrawViewMenu(); break;}
            case 1: { DrawCreateMenu(); break;}
            case 2: { DrawSettingsMenu(); break; }
            default: { break; }
        }
    }

    void CoreWidget::DrawViewMenu()
    {
        constexpr float xOffset = 5.0f;
        const float buttonSizeY = 75.0f * GUI::GetFontScalar();
        constexpr float titleYOffset = 5.0f;
        const float innerXOffset = 40.0f * GUI::GetFontScalar();
        constexpr float innerYOffset = 3.0f;
        const float smallButtonSize = 30.0f * GUI::GetFontScalar();
        constexpr float leftButtonWidth = 75.0f;

        float buttonSizeX = ImGui::GetContentRegionAvail().x - xOffset;

        ImGui::BeginChild("Effects", ImGui::GetContentRegionAvail());
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            ImGui::SeparatorText(ICON_LC_ARROW_DOWN_NARROW_WIDE " FX Line");

            // Draw ONLY enabled effects
            for (auto* fx : mEnabledFX)
            {
                DrawFXBox(fx->GetName().c_str(), buttonSizeX, buttonSizeY, leftButtonWidth,
                          titleYOffset, innerXOffset, innerYOffset, smallButtonSize,
                          [this, fx]()
                          {
                              fx->OnLeftButton();
                              fx->Disable();
                              mPendingMoves.push_back({ fx, false });
                          },
                          [fx]()
                          {
                              fx->OnSettings();
                          }
                );
            }
            ImGui::SeparatorText(ICON_LC_SPEAKER);
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();

        ProcessPendingMoves();
    }

    void CoreWidget::DrawCreateMenu()
    {
        constexpr float xOffset = 5.0f;
        const float buttonSizeY = 75.0f * GUI::GetFontScalar();
        constexpr float titleYOffset = 5.0f;
        const float innerXOffset = 40.0f * GUI::GetFontScalar();
        constexpr float innerYOffset = 3.0f;
        const float smallButtonSize = 30.0f * GUI::GetFontScalar();
        constexpr float leftButtonWidth = 75.0f;

        float buttonSizeX = ImGui::GetContentRegionAvail().x - xOffset;

        ImGui::BeginChild("Effects", ImGui::GetContentRegionAvail());
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            ImGui::SeparatorText(ICON_LC_SPARKLES " Main FX");

            for (auto* fx : mAllFX)
            {
                if (fx->IsEnabled()) continue;

                DrawFXBox(fx->GetName().c_str(), buttonSizeX, buttonSizeY, leftButtonWidth,
                          titleYOffset, innerXOffset, innerYOffset, smallButtonSize,
                          [this, fx]()
                          {
                              fx->OnLeftButton();
                              fx->Enable();
                              mPendingMoves.push_back({ fx, true });
                          },
                          [fx]()
                          {
                              fx->OnSettings();
                          }
                );
            }
            ImGui::SeparatorText(ICON_LC_SPEAKER);
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();

        ProcessPendingMoves();
    }

    void CoreWidget::DrawSettingsMenu()
    {
        const ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
        const float buttonYSize = 75.0f * GUI::GetFontScalar();

        ImGui::BeginChild("##SettingsChild", contentRegionAvail);
        {
            if (ImGui::Button(ICON_LC_BLUETOOTH " Connect", ImVec2(contentRegionAvail.x, buttonYSize)))
            {
                ImGui::OpenPopup("ConnectModal");
            }

            ImGui::Separator();

            if (ImGui::Button(ICON_LC_INFO " Info", ImVec2(contentRegionAvail.x, buttonYSize)))
            {
                ImGui::OpenPopup("InfoModal");
            }

            ImGui::Separator();

            if (ImGui::Button(ICON_LC_LOG_OUT " Exit", ImVec2(contentRegionAvail.x, buttonYSize)))
            {
                mApp->Quit();
            }

            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            // popup for connect
            if (ImGui::BeginPopupModal("ConnectModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove))
            {
                ImGui::SeparatorText(ICON_LC_BLUETOOTH_SEARCHING " List of devices");

                // close
                if (WidgetCentered(ImGui::Button("Close", ImVec2(150, 0)))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            // popup for info
            if (ImGui::BeginPopupModal("InfoModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove))
            {
                // about
                ImGui::SeparatorText("About");
                ImGui::Text("Designed to control the FXServer pedalboard.");
                ImGui::Text("1) Connect FXClient with FXServer via menu.");
                ImGui::Text("2) Add FXs and modify them.");

                // build
                char buildDate[32];
                FormatCompilationVersion(GetCompilationVersion(), buildDate, sizeof(buildDate));

                ImGui::SeparatorText("Build");
                ImGui::Text("Version: 1");
                ImGui::Text("Date: %s", buildDate);

                // credits

                ImGui::SeparatorText("Credits");
                ImGui::Text("Developed by");
                ImGui::SameLine();
                ImGui::TextLinkOpenURL("franzpedd", "https://franzpedd.me/");
                ImGui::SameLine();
                ImGui::Text("using");
                ImGui::SameLine();
                ImGui::TextLinkOpenURL("Cosmos Engine", "https://github.com/cosmos");

                // close
                if (WidgetCentered(ImGui::Button("Close", ImVec2(150, 0)))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::EndChild();
    }

    void CoreWidget::DrawFXBox(const char* effectName, float boxWidth, float boxHeight, float leftButtonWidth, float titleYOffset, float innerXOffset, float innerYOffset, float smallButtonSize, const std::function<void()>& onLeftButtonClick, const std::function<void()>& onSettingsClick)
    {
        const float contentRegionAvailY = ImGui::GetContentRegionAvail().y;
        const float centerYPos = (contentRegionAvailY / 2.0f) - innerYOffset;

        // unique ID using the fx name
        std::string childId = std::string("##") + effectName + "_child";
        std::string settingsId = std::string("##") + effectName + "_settings";

        ImGui::BeginChild(childId.c_str(), ImVec2(boxWidth, boxHeight));
        {
            // left button
            if (ImGui::Button(ICON_LC_CHEVRONS_LEFT "", ImVec2(leftButtonWidth, boxHeight))) {
                if (onLeftButtonClick) onLeftButtonClick();
            }

            ImGui::SameLine();

            // center text
            ImGui::SetCursorPosY(titleYOffset);
            std::string titleText = std::string(ICON_LC_AUDIO_WAVEFORM) + " " + effectName;
            WidgetCentered(ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "%s", titleText.c_str()));

            // settings button
            ImGui::SetCursorPosX(boxWidth - innerXOffset);
            ImGui::SetCursorPosY((ImGui::GetContentRegionAvail().y / 2.0f) - innerYOffset);

            std::string settingsButtonId = std::string(ICON_LC_SETTINGS) + settingsId;
            if (ImGui::Button(settingsButtonId.c_str(), ImVec2(smallButtonSize, smallButtonSize))) {
                if (onSettingsClick) onSettingsClick();
            }
        }
        ImGui::EndChild();
    }

    void CoreWidget::ProcessPendingMoves()
    {
        for (const auto& cmd : mPendingMoves)
        {
            if (cmd.addToEnabled)
            {
                // add to enabled if not already there
                auto it = std::find(mEnabledFX.begin(), mEnabledFX.end(), cmd.fx);
                if (it == mEnabledFX.end()) {
                    mEnabledFX.push_back(cmd.fx);
                }
            }

            else
            {
                // remove from enabled
                auto it = std::find(mEnabledFX.begin(), mEnabledFX.end(), cmd.fx);
                if (it != mEnabledFX.end()) {
                    mEnabledFX.erase(it);
                }
            }
        }
        mPendingMoves.clear();
    }
}