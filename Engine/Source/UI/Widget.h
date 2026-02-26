#pragma once

#include "Core/Defines.h"
#include "Core/Input.h"
#include <vecmath/vecmath.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

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

	class Control
	{
	public:
		
		/// @brief constructor
		Control(ImVec2 windowSize) : mWindowSize(windowSize) {}

		/// @brief draws and returns bool operator overload (ImGui style)
		template<typename Func>
		ControlWrapper operator()(Func control) const
		{
			ImVec2 originalPos = ImGui::GetCursorPos();

			// draw offscreen to calculate size
			ImGui::SetCursorPos(ImVec2(-10000.0f, -10000.0f));

			ImGui::PushID(this);
			control();
			ImGui::PopID();

			ImVec2 controlSize = ImGui::GetItemRectSize();

			// draw at centered position
			ImGui::SetCursorPos(ImVec2((mWindowSize.x - controlSize.x) * 0.5f, originalPos.y));
			control();
			return ControlWrapper(ImGui::IsItemClicked());
		}

	private:
		ImVec2 mWindowSize;
	};

	/// @brief macro to create a centered imgui widget
	#define WidgetCentered(control) Cosmos::Widget::Util::Control { ImGui::GetWindowSize() }([&]() { control; } )
}