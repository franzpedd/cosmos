#include "ResCalcWidget.h"

namespace Cosmos
{
    ResistorCalcWidget::ResistorCalcWidget(ApplicationBase* app)
            : WidgetBase("Main Screen", true), mApp(app)
    {
        mBlack = ConvertRGB_Vec4(0, 0, 0);
        mBrown = ConvertRGB_Vec4(88, 57, 39);
        mRed = ConvertRGB_Vec4(255, 0, 0);
        mOrange = ConvertRGB_Vec4(255, 91, 31);
        mYellow = ConvertRGB_Vec4(255, 255, 0);
        mGreen = ConvertRGB_Vec4(0, 255, 0);
        mBlue = ConvertRGB_Vec4(0, 0, 255);
        mViolet = ConvertRGB_Vec4(148, 0, 211);
        mGrey = ConvertRGB_Vec4(128, 128, 128);
        mWhite = ConvertRGB_Vec4(255, 255, 255);
        mGold = ConvertRGB_Vec4(212, 175, 55);
        mSilver = ConvertRGB_Vec4(165, 169, 180);

        // initial values - Fix: Remove duplicate mBand1 initialization
        mTolerance = BandColor::Brown;
        mBand1 = mBand2 = mBand3 = mMultiplier = mTemperature = BandColor::Black;
    }

    void ResistorCalcWidget::Update()
    {
        static int bandType = 1; // defaulting to 4 bands mode

        if (!mApp) return;

        // set's set a ui-widget-window that covers the framebuffer entirely
        float2 size = mApp->GetWindow()->GeSize();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(size.xy.x, size.xy.y));

        // widget start
        ImGui::Begin("Resistor Calculator", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
        {
            ImVec2 pos = ImGui::GetCursorPos();
            DrawInfo();

            ImGui::SetCursorPos(ImVec2(pos.x + 75.0f, pos.y + 75.0f));
            ImGui::NewLine();

            mResistorInfo.msg.str("");
            mResistorInfo.msg.clear();

            CalculateResistance(bandType);
            WidgetCentered(ImGui::Text("%s", mResistorInfo.msg.str().c_str()));

            ImGui::SeparatorText("");

            ImGui::SeparatorText("Resistor stripes " ICON_FA_QUESTION_CIRCLE);
            ImGui::SetItemTooltip("How many stripes/bands the resistor has");

            // resistor's band mode
            if (ImGui::RadioButton("3 Bands", bandType == 0)) { bandType = 0; }
            ImGui::SameLine();
            if (ImGui::RadioButton("4 Bands", bandType == 1)) { bandType = 1; }
            ImGui::SameLine();
            if (ImGui::RadioButton("5 Bands", bandType == 2)) { bandType = 2; }
            ImGui::SameLine();
            if (ImGui::RadioButton("6 Bands", bandType == 3)) { bandType = 3; }
            ImGui::NewLine();

            switch (bandType)
            {
                case 0:
                {
                    ModeBand(BandFlags_Band1 | BandFlags_Band2 | BandFlags_Multiplier);
                    break;
                }

                case 1:
                {
                    ModeBand(BandFlags_Band1 | BandFlags_Band2 | BandFlags_Multiplier | BandFlags_Tolerance);
                    break;
                }

                case 2:
                {
                    ModeBand(BandFlags_Band1 | BandFlags_Band2 | BandFlags_Band3 | BandFlags_Multiplier | BandFlags_Tolerance);
                    break;
                }

                case 3:
                {
                    ModeBand(BandFlags_Band1 | BandFlags_Band2 | BandFlags_Band3 | BandFlags_Multiplier | BandFlags_Tolerance | BandFlags_Temperature);
                    break;
                }

                default: break;
            }
        }
        ImGui::End();
    }

    void ResistorCalcWidget::ModeBand(BandFlags flags)
    {
        if(flags & BandFlags_Band1)
        {
            ImGui::SeparatorText("1st Color");

            ImGui::PushID("##1stID");
            DrawBand(&mBand1);
            ImGui::PopID();

            ImGui::NewLine();
        }

        if(flags & BandFlags_Band2)
        {
            ImGui::SeparatorText("2nd Color");

            ImGui::PushID("##2ndID");
            DrawBand(&mBand2);
            ImGui::PopID();

            ImGui::NewLine();
        }

        if(flags & BandFlags_Band3)
        {
            ImGui::SeparatorText("3rd Color");  // Fix: Changed "3nd" to "3rd"

            ImGui::PushID("##3rdID");  // Fix: Changed ID to match
            DrawBand(&mBand3);
            ImGui::PopID();

            ImGui::NewLine();
        }

        if(flags & BandFlags_Multiplier)
        {
            ImGui::SeparatorText("Multiplier");

            ImGui::PushID("##MultiplierID");
            DrawBand(&mMultiplier, true, true, true);
            ImGui::PopID();

            ImGui::NewLine();
        }

        if(flags & BandFlags_Tolerance)
        {
            ImGui::SeparatorText("Tolerance");

            ImGui::PushID("##ToleranceID");
            DrawBand(&mTolerance, false, false, true);
            ImGui::PopID();

            ImGui::NewLine();
        }

        if(flags & BandFlags_Temperature)
        {
            ImGui::SeparatorText("Temperature");

            ImGui::PushID("##TemperatureID");
            DrawBand(&mTemperature, true, false, false);
            ImGui::PopID();

            ImGui::NewLine();
        }
    }

    void ResistorCalcWidget::DrawBand(BandColor* option, bool drawBlack, bool drawWhite, bool extraOptions)
    {
        // Helper lambda to create consistent spacing
        auto drawColorOption = [&](BandColor color, const char* label, const float4& bgColor, const float4& textColor, bool enabled = true) {
            ImGui::BeginDisabled(!enabled);
            if(ImGui::RadioButton(("##" + std::string(label)).c_str(), *option == color)) { *option = color; }
            ImGui::SameLine();
            Widget::TextBackground(bgColor, textColor, ("#" + std::string(label) + "Txt").c_str(), label);
            ImGui::SameLine();
            ImGui::EndDisabled();
        };

        // Use fixed-width formatting for alignment
        drawColorOption(BandColor::Black,  "Black ", mBlack, mWhite, drawBlack);
        drawColorOption(BandColor::Brown,  "Brown ", mBrown, mWhite, true);
        drawColorOption(BandColor::Red,    "Red   ", mRed, mWhite, true);
        drawColorOption(BandColor::Orange, "Orange", mOrange, mWhite, true);
        ImGui::NewLine(); // Force new line after 4 items

        drawColorOption(BandColor::Yellow, "Yellow", mYellow, mBlack, true);
        drawColorOption(BandColor::Green,  "Green ", mGreen, mBlack, true);
        drawColorOption(BandColor::Blue,   "Blue  ", mBlue, mWhite, true);
        drawColorOption(BandColor::Violet, "Violet", mViolet, mWhite, true);
        ImGui::NewLine(); // Force new line after 4 more items

        drawColorOption(BandColor::Grey,   "Grey  ", mGrey, mWhite, true);
        drawColorOption(BandColor::White,  "White ", mWhite, mBlack, drawWhite);

        if(extraOptions) {
            drawColorOption(BandColor::Gold,   "Gold  ", mGold, mBlack, true);
            drawColorOption(BandColor::Silver, "Silver", mSilver, mBlack, true);
        }
    }

    void ResistorCalcWidget::DrawInfo()
    {
        const char* title = "ResCalc (Resistor Calculator)";
        if (WidgetCentered(ImGui::Button(title, ImVec2(ImGui::CalcTextSize(title).x + 20.0f, 100.0f)))) { ImGui::OpenPopup("InfoPopup"); }

        ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y - 25.0f);

        if(WidgetCentered(ImGui::Button("Quit", ImVec2(150.0f, 100.0f)))) {
            mApp->Quit();
        }

        ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.1f, 0.15f, 0.2f, 0.5f));
        if (ImGui::BeginPopupModal("InfoPopup", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar)) {
            ImGui::Text("Developed in C++ with Cosmos Engine");
            ImGui::Text("github.com/franzpedd/cosmos");
            if(WidgetCentered(ImGui::Button("Back"))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor();
    }

    void ResistorCalcWidget::CalculateResistance(int bandMode)
    {
        double res = 0.0;

        switch (bandMode)
        {
            case 0: // 3 bands
            {
                int significant = 10 * (int)mBand1 + (int)mBand2;
                double multiplier = GetMultiplierValue(mMultiplier);
                res = significant * multiplier;

                char formatted[32];
                Resistance_Format(res, formatted);
                mResistorInfo.msg << formatted << " ±20%";  // 3-band resistors have ±20% tolerance
                break;
            }

            case 1: // 4 bands
            {
                int significant = 10 * (int)mBand1 + (int)mBand2;
                double multiplier = GetMultiplierValue(mMultiplier);
                res = significant * multiplier;

                char formatted[32];
                Resistance_Format(res, formatted);
                mResistorInfo.msg << formatted << " ±" << GetToleranceValue(mTolerance) << "%";
                break;
            }

            case 2: // 5 bands
            {
                int significant = 100 * (int)mBand1 + 10 * (int)mBand2 + (int)mBand3;
                double multiplier = GetMultiplierValue(mMultiplier);
                res = significant * multiplier;

                char formatted[32];
                Resistance_Format(res, formatted);
                mResistorInfo.msg << formatted << " ICON_LC_DIFF" << GetToleranceValue(mTolerance) << "%";
                break;
            }

            case 3: // 6 bands
            {
                int significant = 100 * (int)mBand1 + 10 * (int)mBand2 + (int)mBand3;
                double multiplier = GetMultiplierValue(mMultiplier);
                res = significant * multiplier;

                char formatted[32];
                Resistance_Format(res, formatted);
                mResistorInfo.msg << formatted << " " << ICON_LC_DIFF << GetToleranceValue(mTolerance) << "%  "
                                  << GetTemperatureCoefficient(mTemperature);
                break;
            }

            default: break;
        }
    }

    double ResistorCalcWidget::GetMultiplierValue(BandColor color)
    {
        switch (color)
        {
            case Black:  return 1.0;
            case Brown:  return 10.0;
            case Red:    return 100.0;
            case Orange: return 1000.0;
            case Yellow: return 10000.0;
            case Green:  return 100000.0;
            case Blue:   return 1000000.0;
            case Violet: return 10000000.0;
            case Grey:   return 100000000.0;
            case White:  return 1000000000.0;
            case Gold:   return 0.1;
            case Silver: return 0.01;
            default:     return 1.0;
        }
    }

    const char* ResistorCalcWidget::GetToleranceValue(BandColor color)
    {
        switch (color)
        {
            case Brown:  return "1";
            case Red:    return "2";
            case Orange: return "0.05";
            case Yellow: return "0.02";
            case Green:  return "0.5";
            case Blue:   return "0.25";
            case Violet: return "0.1";
            case Grey:   return "0.01";
            case Gold:   return "5";
            case Silver: return "10";
            default:     return "";
        }
    }

    const char* ResistorCalcWidget::GetTemperatureCoefficient(BandColor color)
    {
        switch (color)
        {
            case Black:  return "250 ppm/K";
            case Brown:  return "100 ppm/K";
            case Red:    return "50 ppm/K";
            case Orange: return "15 ppm/K";
            case Yellow: return "25 ppm/K";
            case Green:  return "20 ppm/K";
            case Blue:   return "10 ppm/K";
            case Violet: return "5 ppm/K";
            case Grey:   return "1 ppm/K";
            default:     return "";
        }
    }

    void ResistorCalcWidget::Resistance_Format(double ohms, char* output)
    {
        if (ohms >= 1e9)
            sprintf(output, "%.2f G" ICON_LC_OMEGA, ohms / 1e9);
        else if (ohms >= 1e6)
            sprintf(output, "%.2f M" ICON_LC_OMEGA, ohms / 1e6);
        else if (ohms >= 1e3)
            sprintf(output, "%.2f k" ICON_LC_OMEGA, ohms / 1e3);
        else if (ohms >= 1.0)
            sprintf(output, "%.2f " ICON_LC_OMEGA, ohms);
        else if (ohms >= 0.001)
            sprintf(output, "%.2f m" ICON_LC_OMEGA, ohms * 1000);  // milliohms
        else
            sprintf(output, "%.2f " ICON_LC_OMEGA, ohms);  // fallback
    }

    float4 ResistorCalcWidget::ConvertRGB_Vec4(int r, int g, int b)
    {
        return {(float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f};
    }
}