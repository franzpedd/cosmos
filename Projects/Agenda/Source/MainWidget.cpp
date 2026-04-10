#include "MainWidget.h"
#include <filesystem>

namespace Cosmos::Agenda
{
    MainWidget::MainWidget(const ApplicationBase* app)
        : WidgetBase("Main Widget", true), mApp(app)
    {
        mPickerColor[0] = mPickerColor[1] = mPickerColor[2] = mPickerColor[3] = 1.0f;

        std::string datafilePath = std::filesystem::path(mApp->GetAssetsPath()).append("datafile.json").string();
        if (std::filesystem::exists(datafilePath)) {
            Datafile::Read(mNotesDF, datafilePath);
            LoadNotesFromDatafile();
        }
    }

    void MainWidget::LoadNotesFromDatafile()
    {
        // not a valid datafile, not loading anything
        if (!mNotesDF.Exists("NotesIndex/Count")) {
            return;
        }

        // get the number of date entries
        int dateCount = mNotesDF["NotesIndex/Count"].GetInt();

        for (int dateIdx = 1; dateIdx <= dateCount; dateIdx++)
        {
            // get the date key from the index
            std::string indexPath = "NotesIndex/" + std::to_string(dateIdx);
            if (!mNotesDF.Exists(indexPath)) {
                continue;
            }

            std::string dateKey = mNotesDF[indexPath].GetString();

            // check if the date data exists
            std::string countPath = "Notes/" + dateKey + "/Count";
            if (!mNotesDF.Exists(countPath)) {
                continue;
            }

            int count = mNotesDF[countPath].GetInt();

            // parse date from key
            size_t firstSep = dateKey.find("::");
            size_t secondSep = dateKey.find("::", firstSep + 2);

            if (firstSep == std::string::npos || secondSep == std::string::npos) {
                continue; // invalid key format
            }

            uint32_t year = std::stoi(dateKey.substr(0, firstSep));
            std::string monthStr = dateKey.substr(firstSep + 2, secondSep - (firstSep + 2));
            Month month = string_to_month(monthStr);
            uint8_t dayNum = std::stoi(dateKey.substr(secondSep + 2));

            // calculate actual day of week
            Day dayOfWeek = calculate_day_of_week(year, month, dayNum);

            // clear any existing notes for this date (in case of reload)
            mNotes[dateKey].clear();

            for (int i = 1; i <= count; i++)
            {
                std::string noteNumStr = std::to_string(i);
                std::string noteBasePath = "Notes/" + dateKey + "/Notes/" + noteNumStr;

                // check if the note text exists
                std::string textPath = noteBasePath + "/Text";
                if (!mNotesDF.Exists(textPath)) {
                    continue;
                }

                float4 color;
                color.xyzw.x = mNotesDF.Exists(noteBasePath + "/R") ? mNotesDF[noteBasePath + "/R"].GetDouble() : 1.0f;
                color.xyzw.y = mNotesDF.Exists(noteBasePath + "/G") ? mNotesDF[noteBasePath + "/G"].GetDouble() : 1.0f;
                color.xyzw.z = mNotesDF.Exists(noteBasePath + "/B") ? mNotesDF[noteBasePath + "/B"].GetDouble() : 1.0f;
                color.xyzw.w = mNotesDF.Exists(noteBasePath + "/A") ? mNotesDF[noteBasePath + "/A"].GetDouble() : 1.0f;

                Note note( dayOfWeek, dayNum, month, year, mNotesDF[textPath].GetString(), color);
                mNotes[dateKey].push_back(note);
            }

            // verify that we loaded the correct number of notes
            if (mNotes[dateKey].size() != static_cast<size_t>(count)) {
                LOG_TO_TERMINAL(Cosmos::Logger::Warn, "Warning: Expected %d notes for %s, but loaded %zu\n", count, dateKey.c_str(), mNotes[dateKey].size())
            }
        }
    }

    void MainWidget::SaveNotesToDatafile()
    {
        int dateIndex = 1;
        Datafile newDF;

        for (const auto& [dateKey, notes] : mNotes) {
            newDF["NotesIndex/" + std::to_string(dateIndex)].SetString(dateKey);
            newDF["Notes/" + dateKey + "/Count"].SetInt(static_cast<int>(notes.size()));

            // save each note
            int noteNum = 1;
            for (const auto& note : notes)
            {
                std::string noteNumStr = std::to_string(noteNum);
                std::string basePath = "Notes/" + dateKey + "/Notes/" + noteNumStr;

                newDF[basePath + "/Text"].SetString(note.text);
                newDF[basePath + "/R"].SetDouble(note.color.xyzw.x);
                newDF[basePath + "/G"].SetDouble(note.color.xyzw.y);
                newDF[basePath + "/B"].SetDouble(note.color.xyzw.z);
                newDF[basePath + "/A"].SetDouble(note.color.xyzw.w);
                noteNum++;
            }

            dateIndex++;
        }

        // save the total count of dates
        newDF["NotesIndex/Count"].SetInt(dateIndex - 1);

        // replace the old Datafile
        mNotesDF = std::move(newDF);

        // write to file (overwrites existing file)
        std::string filePath = std::filesystem::path(mApp->GetAssetsPath()).append("datafile.json").string();
        Datafile::Write(mNotesDF, filePath);
    }

    void MainWidget::Update()
    {
        ImGui::DockSpaceOverViewport();
        DisplayMenubar();
        DisplayCalendarWidget();
        DisplayNotesWidget();
        DisplayDayClickPopup();
    }

    void MainWidget::Render()
    {
    }

    void MainWidget::DisplayMenubar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu(ICON_LC_PALETTE " Tema"))
            {
                GUI::Style currentStyle = mApp->GetGUI()->GetCurrentStyle();

                if (ImGui::RadioButton("Classic", currentStyle == GUI::Style::Classic)) { mApp->GetGUI()->SetStyle(GUI::Style::Classic); }
                if (ImGui::RadioButton("Darkish", currentStyle == GUI::Style::Darkish)) { mApp->GetGUI()->SetStyle(GUI::Style::Darkish); }
                if (ImGui::RadioButton("Dark", currentStyle == GUI::Style::Dark)) { mApp->GetGUI()->SetStyle(GUI::Style::Dark); }
                if (ImGui::RadioButton("Light", currentStyle == GUI::Style::Light)) { mApp->GetGUI()->SetStyle(GUI::Style::Light); }
                if (ImGui::RadioButton("Sunset", currentStyle == GUI::Style::Sunset)) { mApp->GetGUI()->SetStyle(GUI::Style::Sunset); }
                if (ImGui::RadioButton("Purple Haze", currentStyle == GUI::Style::PurpleHaze)) { mApp->GetGUI()->SetStyle(GUI::Style::PurpleHaze); }
                if (ImGui::RadioButton("GoldSrc", currentStyle == GUI::Style::GoldSrc)) { mApp->GetGUI()->SetStyle(GUI::Style::GoldSrc); }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        
    }

    void MainWidget::DisplayCalendarWidget()
    {
        static bool scrolledToCurrentMonth = false;
        const ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
        const ImVec2 monthsAreaSize = ImVec2(0, 290);
        const ImVec2 monthBoxSize = ImVec2(300, 250);

        const uint8_t days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        const Month month[] = { January, February, March, April, May, June, July, August, September, October, November, December };
        const Day startDay[] = { Thursday, Sunday, Sunday, Wednesday, Friday, Monday, Wednesday, Saturday, Tuesday, Thursday, Monday, Tuesday };

        if (ImGui::Begin(ICON_LC_CALENDAR " Calendário", nullptr, flags))
        {
            ImGui::BeginChild("##Months", monthsAreaSize, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            {
                for (uint8_t i = 0; i < 12; i++)
                {
                    DrawMonth(month[i], days[i], startDay[i], monthBoxSize);
                    ImGui::SameLine();
                    Widget::Separator(true, 2.0f);
                    if (i < 11) ImGui::SameLine();
                }

                // scroll to the current month ("centralized")
                if (!scrolledToCurrentMonth) {
                    ImGui::SetScrollX((monthBoxSize.x * (GetMonthNumber() - 2.5)));
                    scrolledToCurrentMonth = true;
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void MainWidget::DisplayNotesWidget()
    {
        if (ImGui::Begin(ICON_LC_PEN " Notes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            std::vector<DateComponents> sortedDates = GetSortedData();

            for (const auto& date : sortedDates)
            {
                const auto& notes = mNotes[date.originalKey];
                std::string dateKey = date.originalKey;

                // date separator
                char dateStr[64];
                const ImVec4 separatorColor = ImVec4(0.8f, 0.5f, 0.2f, 1.0f);
                snprintf(dateStr, sizeof(dateStr), "##%s", dateKey.c_str());
                ImGui::SeparatorText(dateStr);
                ImGui::TextColored(separatorColor, "%s %d, %d",  month_c_str(static_cast<Month>(date.month), true), date.day, date.year);

                // list each note
                int noteIndex = 1;
                for (auto it = notes.begin(); it != notes.end(); ++it, noteIndex++)
                {
                    const auto& note = *it;
                    ImGui::PushID(date.originalKey.c_str());
                    ImGui::PushID(noteIndex);

                    // delete button
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                    if (ImGui::SmallButton(ICON_LC_TRASH ""))
                    {
                        mNotes[dateKey].erase(it);

                        if (mNotes[dateKey].empty()) {
                            mNotes.erase(dateKey);
                        }

                        SaveNotesToDatafile();

                        ImGui::PopStyleColor(2);
                        ImGui::PopID();
                        ImGui::PopID();

                        break;
                    }

                    ImGui::PopStyleColor(2);

                    ImGui::SameLine();

                    // note
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(note.color.xyzw.x, note.color.xyzw.y, note.color.xyzw.z, note.color.xyzw.w));
                    ImGui::Text(": %s", note.text.c_str());
                    ImGui::PopStyleColor();

                    ImGui::PopID();
                    ImGui::PopID();
                }
                ImGui::Spacing();
            }
        }
        ImGui::End();
    }

    void MainWidget::DisplayDayClickPopup()
    {
        if (mDayPopup.displaying) {
            ImGui::OpenPopup(ICON_LC_CALENDAR_COG " Editar");
            mDayPopup.displaying = false;
        }

        if (ImGui::BeginPopupModal(ICON_LC_CALENDAR_COG " Editar", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            char str[32];
            snprintf(str, sizeof(str), "%s - Feira %u/%u/%d", day_c_str(mDayPopup.weekDay), mDayPopup.numberDay, (int)mDayPopup.month, GetYearNumber());

            ImGui::SeparatorText(str);
            ImGui::Separator();

            ImGui::Text(ICON_LC_PEN " Adicionar Nota");

            static char buffer[1024];
            ImGui::InputTextMultiline("##AddNote", buffer, sizeof(buffer), ImVec2(525, 150), ImGuiInputTextFlags_None);

            ImGui::SameLine();

            if (ImGui::Button("Add", ImVec2(75.0f, 150)))
            {
                if (!std::string(buffer).empty()) {
                    mDayPopup.color.xyzw.x = mPickerColor[0];
                    mDayPopup.color.xyzw.y = mPickerColor[1];
                    mDayPopup.color.xyzw.z = mPickerColor[2];
                    mDayPopup.color.xyzw.w = mPickerColor[3];

                    std::string dateKey = make_date_key(GetYearNumber(), mDayPopup.month, mDayPopup.numberDay);
                    Note note = Note(mDayPopup.weekDay, mDayPopup.numberDay, mDayPopup.month, GetYearNumber(), std::string(buffer), mDayPopup.color);

                    mNotes[dateKey].push_back(note);
                    SaveNotesToDatafile();
                    buffer[0] = '\0';
                }
            }

            ImGui::Separator();

            ImGui::Text(ICON_LC_PAINT_BUCKET " Cor");

            if (ImGui::ColorEdit4("##ColorPicker", mPickerColor, ImGuiColorEditFlags_DisplayRGB));

            ImGui::Separator();

            WidgetGroupCentered
            (
                [&]()
                {
                    if (ImGui::Button("Voltar", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                    }
                }
            );

            ImGui::EndPopup();
        }
    }

    void MainWidget::DrawMonth(Month month, uint8_t days, Day startDay, ImVec2 boxSize)
    {
        const ImVec2 buttonSize = ImVec2(35, 25);

        char childId[64];
        snprintf(childId, sizeof(childId), "##%s_%p", month_c_str(month), this);

        ImGui::BeginChild(childId, boxSize, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX);
        {
            ImGui::SeparatorText(month_c_str(month));

            ImGui::BeginDisabled();
            {
                const char* weekdays[] = { "Seg", "Ter", "Qua", "Qui", "Sex", "Sab", "Dom" };
                for (int i = 0; i < 7; i++) {
                    ImGui::Button(weekdays[i], ImVec2(buttonSize.x, 0));
                    if (i < 6) ImGui::SameLine();
                }

                ImGui::Separator();

                for (int i = 0; i < (int)startDay; i++) {
                    ImGui::Dummy(buttonSize);
                    ImGui::SameLine();
                }
            }
            ImGui::EndDisabled();

            for (uint8_t i = 1; i <= days; i++) {
                char buff[4];
                snprintf(buff, sizeof(buff), "%2u", i);

                if ((i + (int)startDay - 1) % 7 != 0 && i > 1) {
                    ImGui::SameLine();
                }

                // check if this day has notes
                std::string fullDateKey = make_date_key(GetYearNumber(), month, i);
                bool hasNotes = mNotes.find(fullDateKey) != mNotes.end() && !mNotes[fullDateKey].empty();

                if (hasNotes) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));
                }

                if (ImGui::Button(buff, buttonSize)) {
                    mDayPopup.displaying = true;
                    mDayPopup.numberDay = i;
                    mDayPopup.weekDay = startDay;
                    mDayPopup.month = month;
                }

                if (hasNotes) {
                    ImGui::PopStyleColor();
                }
            }
        }
        ImGui::EndChild();
    }

    std::vector<DateComponents> MainWidget::GetSortedData() const
    {
        std::vector<DateComponents> sortedDates;
        sortedDates.reserve(mNotes.size());

        for (const auto& [dateKey, notes] : mNotes)
        {
            DateComponents dc;
            dc.originalKey = dateKey;

            // parse the date key "2026::April::30"
            size_t firstSep = dateKey.find("::");
            size_t secondSep = dateKey.find("::", firstSep + 2);

            if (firstSep != std::string::npos && secondSep != std::string::npos)
            {
                dc.year = std::stoi(dateKey.substr(0, firstSep));
                std::string monthStr = dateKey.substr(firstSep + 2, secondSep - (firstSep + 2));
                dc.month = month_name_to_number(monthStr);
                dc.day = std::stoi(dateKey.substr(secondSep + 2));
                sortedDates.push_back(dc);
            }
        }

        std::sort(sortedDates.begin(), sortedDates.end(),
            [](const DateComponents& a, const DateComponents& b) {
                if (a.year != b.year) return a.year < b.year;
                if (a.month != b.month) return a.month < b.month;
                return a.day < b.day;
            });

        return sortedDates;
    }
}