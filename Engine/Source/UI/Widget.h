#pragma once

#include "Core/Defines.h"
#include "Core/Input.h"
#include <vecmath/vecmath.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <algorithm>

namespace Cosmos
{
	class COSMOS_API WidgetBase
	{
	public:

		// constructor
		WidgetBase(const char* name, bool visible = false) : mName(name), mVisible(visible) {};

		// destructor
		virtual ~WidgetBase() = default;

		// returns it's name
		inline const char* GetName() { return mName; }

		// returns if the widget is visible/displaying
		inline bool GetVisibility() const { return mVisible; }

		// sets the widget visibility
		inline void SetVisibility(bool value) { mVisible = value; }

	public:

		// ui drawing
		inline virtual void Update() {};

		// renderer drawing
		inline virtual void Render() {};

	public:

		/// @brief called from window when it gets minimized
		inline virtual void OnMinimize() {};

		/// @brief called from window when it gets maximized
		inline virtual void OnMaximize() {};

		/// @brief called from window when it's size is restored
		inline virtual void OnRestore() {};

		/// @brief called from window when it's size changes
		inline virtual void OnResize(int32_t width, int32_t height) {};

		/// @brief called from window when a keyboard press is triggered
		inline virtual void OnKeyPress(Input::Keycode key, Input::Keymod mod, bool held) {};

		/// @brief called from window when a keyboard release is triggered
		inline virtual void OnKeyRelease(Input::Keycode keycode) {};

		/// @brief called from window when a mouse press is triggered
		inline virtual void OnButtonPress(Input::Buttoncode buttoncode, Input::Keymod mod) {};

		/// @brief called from window when a mouse release is triggered
		inline virtual void OnButtonRelease(Input::Buttoncode buttoncode) {};

		/// @brief called from window when a mouse 'scroll' is triggered
		inline virtual void OnMouseScroll(double xoffset, double yoffset) {};

		/// @brief called from window when a mouse move is triggered
		inline virtual void OnMouseMove(double xoffset, double yoffset) {};

		/// @brief called from window when dpi change is triggered
		inline virtual void OnDPIChange(float scale) {};

	protected:

		const char* mName = nullptr;
		bool mVisible = false;
	};
}

namespace Cosmos::Widget
{
	/// @brief centered text in the window
	/// @param fmt va_arglist to format the text
	COSMOS_API void TextCentered(const char* fmt, ...);

	/// @brief little hack that transforms a table into a text with colored background
	/// @param bgCol the background color 0-255 for each value
	/// @param txtCol the text color 0-255 for each value
	/// @param label the text id/text
	/// @param fmt va_arglist to format the text
	COSMOS_API void TextBackground(float4 bgCol, float4 txtCol, const char* label, const char* fmt, ...);

	/// @brief custom checkbox
	/// @param label checkbox text
	/// @param v controls the checkbox is on/off
	/// @return true enabled, false on disabled
	COSMOS_API bool Checkbox(const char* label, bool* v);

	/// @brief custom checkbox slider
	/// @param label checkbox text
	/// @param v controls the checkbox is on/off
	/// @return true enabled, false on disabled
	COSMOS_API bool CheckboxSliderEx(const char* label, bool* v);

	/// @brief custom float controller
	/// @param label label control text
	/// @param x controller value
	COSMOS_API void FloatControl(const char* label, float* value);

	/// @brief custom 2d float controller
	/// @param label label control text
	/// @param x first controller value
	/// @param y second controller value
	COSMOS_API void Float2Control(const char* label, float* x, float* y);

	/// @brief custom 3d float controller
	/// @param label label control text
	/// @param x first controller value
	/// @param y second controller value
	/// @param z third controller value
	COSMOS_API void Float3Controller(const char* label, float* x, float* y, float* z);

	/// @brief draws a button with custom color when selected
	/// @param label label control text
	/// @param optionid the id of the button
	/// @param selectedoptionid address to the selected button holder
	/// @param selectedcolor the color of the button when selected
	/// @param color the button default color
	/// @param size the button size
	COSMOS_API bool ButtonOption(const char* label, int32_t optionid, int32_t* selectedoptionid, const ImVec4& selectedColor, const ImVec4& color, const ImVec2& size = ImVec2(0.0f, 0.0f));
}

namespace Cosmos::Widget::Util
{
	class ControlWrapper
	{
	public:

		/// @brief constructor
		explicit ControlWrapper(bool result) : mResult(result) {}

		/// @brief returns the result
		operator bool() const { return mResult; }

	private:
		bool mResult;
	};

	class ControlID
	{
	public:

		/// @brief constructor
		ControlID() { ImGui::PushID(this); }

		/// @brief destructor
		~ControlID() { ImGui::PopID(); }

		/// @brief delete copy constructor
		ControlID(const ControlID&) = delete;

		/// @brief delete assignment constructor
		ControlID& operator=(const ControlID&) = delete;
	};

	class LineGroup
	{
	public:

		/// @brief constructor
		LineGroup() : mItemCount(0), mMinX(FLT_MAX), mMaxX(-FLT_MAX), mMinY(FLT_MAX), mMaxY(-FLT_MAX) {}

		/// @brief add items into the line group (for measurement)
		template<typename Func>
		LineGroup& AddForMeasure(Func control)
		{
			ImVec2 beforePos = ImGui::GetCursorPos();

			if (mItemCount > 0) { ImGui::SameLine(); }

			control();

			// update bounding box
			ImVec2 itemMin = ImGui::GetItemRectMin();
			ImVec2 itemMax = ImGui::GetItemRectMax();

			mMinX = std::min(mMinX, itemMin.x);
			mMaxX = std::max(mMaxX, itemMax.x);
			mMinY = std::min(mMinY, itemMin.y);
			mMaxY = std::max(mMaxY, itemMax.y);

			mItemCount++;
			return *this;
		}

		/// @brief add items into the line group (for drawing)
		template<typename Func>
		LineGroup& AddForDraw(Func control, float startX, float originalY)
		{
			ImGui::SetCursorPos(ImVec2(startX, originalY));

			for (int i = 0; i < mItemCount; i++)
			{
				if (i > 0)
				{
					// move cursor to next position based on previous item's size
					ImVec2 lastItemSize = ImGui::GetItemRectSize();
					startX += lastItemSize.x + ImGui::GetStyle().ItemSpacing.x;
					ImGui::SetCursorPos(ImVec2(startX, originalY));
				}
				control();
			}

			return *this;
		}

		/// @brief returns how many items are in the line group
		int GetItemCount() const { return mItemCount; }

		/// @brief returns the total width of all items (including spacing)
		float GetTotalWidth() const
		{
			if (mItemCount <= 1) { return mMaxX - mMinX; }
				
			// add spacing between items
			return (mMaxX - mMinX) + (ImGui::GetStyle().ItemSpacing.x * (mItemCount - 1));
		}

	private:
		int mItemCount;
		float mMinX, mMaxX, mMinY, mMaxY;
	};

	class Control
	{
	public:

		/// @brief constructor
		Control(ImVec2 windowSize) : mWindowSize(windowSize) {}

		/// operator overload for single controls
		template<typename Func>
		static ControlWrapper Centered(Func control)
		{
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 originalPos = ImGui::GetCursorPos();

			// draw offscreen to calculate size
			ImGui::SetCursorPos(ImVec2(-10000.0f, -10000.0f));

			// use a unique id based on the function address to avoid conflicts
			ImGui::PushID(&control);
			control();
			ImGui::PopID();

			ImVec2 controlSize = ImGui::GetItemRectSize();

			// draw at centered position
			ImGui::SetCursorPos(ImVec2((windowSize.x - controlSize.x) * 0.5f, originalPos.y));
			control();
			return ControlWrapper(ImGui::IsItemClicked());
		}

		/// @brief static method for multiple controls
		template<typename... Controls>
		static bool CenteredLine(Controls&&... controls)
		{
			ControlID id;
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 originalPos = ImGui::GetCursorPos();

			// first pass: neasure offscreen - track the bounding box of ALL items
			ImGui::SetCursorPos(ImVec2(-10000.0f, -10000.0f));

			// track the min and max X positions of all items
			float minX = FLT_MAX;
			float maxX = -FLT_MAX;
			int count = 0;

			([&]()
				{
					if (count > 0) ImGui::SameLine();

					ImGui::PushID(count * 2);  // measurement pass gets even IDs
					std::forward<Controls>(controls)();
					ImGui::PopID();

					// get the actual item rectangle and update bounds
					ImVec2 itemMin = ImGui::GetItemRectMin();
					ImVec2 itemMax = ImGui::GetItemRectMax();

					minX = std::min(minX, itemMin.x);
					maxX = std::max(maxX, itemMax.x);

					count++;
				}(), ...);

			// calculate total width (including the natural spacing from SameLine)
			float totalWidth = maxX - minX;

			// second pass: draw centered
			float startX = (windowSize.x - totalWidth) * 0.5f;
			ImGui::SetCursorPos(ImVec2(startX, originalPos.y));

			count = 0;
			([&]()
				{
					if (count > 0) ImGui::SameLine();

					ImGui::PushID((count * 2) + 1);  // display pass gets odd ids
					std::forward<Controls>(controls)();
					ImGui::PopID();

					count++;
				}(), ...);

			return ImGui::IsItemClicked();
		}

	private:
		ImVec2 mWindowSize;
	};

	/// @brief macro to create a centered imgui widget
	#define WidgetCentered(control) Cosmos::Widget::Util::Control::Centered([&]() { control; })

	/// @brief macro to create a centered imgui widget group
	#define WidgetGroupCentered(...) Cosmos::Widget::Util::Control::CenteredLine(__VA_ARGS__)
}