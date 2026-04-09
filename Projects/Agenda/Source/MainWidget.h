#pragma once

#include <Engine.h>

namespace Cosmos
{
	enum Day
	{
		Monday = 0, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
	};

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

		/// @brief displays the widget window
		void DisplayMainWidget();

	private:

		/// @brief displays a sub-widget for the month 
		void DrawMonth(const char* month, uint8_t days, Day startDay, ImVec2 pivotPos = ImVec2(0, 0));

	private:
		const ApplicationBase* mApp = nullptr;
	};
}