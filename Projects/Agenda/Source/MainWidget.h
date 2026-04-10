#pragma once

#include <Engine.h>
#include "Util.h"

namespace Cosmos::Agenda
{
	class MainWidget : public WidgetBase
	{
	public:

		/// @brief constructor
		MainWidget(const ApplicationBase* app);

		/// @brief destructor
		virtual ~MainWidget() = default;

	public:

		/// @brief updates the frame logic
		virtual void Update() override;

		/// @brief updates the render logic
		virtual void Render() override;

	private:

		/// @brief displays the menubar widget
		void DisplayMenubar();

		/// @brief displays the main widget window
		void DisplayCalendarWidget();

		/// @brief displays the notes widget window
		void DisplayNotesWidget();

		/// @brief displays the popup when clicked on a day
		void DisplayDayClickPopup();

		/// @brief loads notes from Datafile into mNotes map
		void LoadNotesFromDatafile();

		/// @brief saves all notes from mNotes map to Datafile
		void SaveNotesToDatafile();

	private:

		/// @brief displays a sub-widget for the month 
		void DrawMonth(Month month, uint8_t days, Day startDay, ImVec2 boxSize = ImVec2(0, 0));

		/// @brief returns the data as month-sorted
		std::vector<DateComponents> GetSortedData() const;

	private:
		const ApplicationBase* mApp = nullptr;

		struct DayPopupInfo
		{
			bool displaying = false;
			Month month = January;
			Day weekDay = Monday;
			uint8_t numberDay = 1;
			float4 color;

			/// @brief constructor
			DayPopupInfo() { color.xyzw.x = color.xyzw.y = color.xyzw.z = 0.0f; color.xyzw.w = 1.0f; }

		} mDayPopup;

		Datafile mNotesDF;
		std::unordered_map<std::string, std::vector<Note>> mNotes;  // date_key -> vector of notes
		float mPickerColor[4];
	};
}