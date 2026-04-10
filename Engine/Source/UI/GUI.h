#pragma once

#include "Core/Defines.h"
#include "Core/Input.h"
#include "Util/Container.h"
#include "Util/Library.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

/// @brief forward declarations
namespace Cosmos { class ApplicationBase; }
namespace Cosmos { class WidgetBase; }

namespace Cosmos
{
    class COSMOS_API GUI
    {
	public:

		enum Style
		{
			Classic = 0, Darkish, Dark, Light, Sunset, PurpleHaze, GoldSrc
		};

    public:

		/// @brief constructor
		GUI(ApplicationBase* app, Style style = Classic);

		/// @brief destructor
		virtual ~GUI();

		/// @brief returns the imgui context
		static void* GetImGuiContext();

		/// @brief returns the text font size, useful for adjusting UI text
		static float GetTextFontSize();

		/// @brief returns the icon font Y size, useful for adjusting UI text
		static float GetIconFontSize();

		/// @brief returns the font scalar for creating ui widgets, on android things should be larger than desktop
		static float GetFontScalar();

		/// @brief returns the current gui style
		inline Style GetCurrentStyle() { return mCurrentStyle; };

	public:

		/// @brief process all logic events
		void Update();

		/// @brief process all rendering events
		void Render();

		/// @brief renders the raw data/VkCommandBuffer
		void DrawRawData(void* commandbuffer);

		/// @brief adds a new widget into the ui
		bool AddWidget(WidgetBase* widget);

		/// @brief removes a widget from the ui
		bool RemoveWidget(const char* name);

		/// @brief returns the widget by it's name
		WidgetBase* FindWidget(const char* name);

		/// @brief enables/disables the cursor
		void ToggleCursor();

		/// @brief adds an image 
		void AddDescriptorImage(const char* key, void* sampler, void* view);

		/// @brief returns the descriptor set via it's key
		void* GetDescriptorImage(const char* key);

    public:

		/// @brief called from window when it gets minimized
		virtual void OnMinimize();

		/// @brief called from window when it gets maximized
		virtual void OnMaximize();

		/// @brief called from window when it's size is restored
		virtual void OnRestore();

		/// @brief called from window when it's size changes
		virtual void OnResize(int32_t width, int32_t height);

		/// @brief called from window when a keyboard press is triggered
		virtual void OnKeyPress(Input::Keycode keycode, Input::Keymod mod, bool held);

		/// @brief called from window when a keyboard release is triggered
		virtual void OnKeyRelease(Input::Keycode keycode);

		/// @brief called from window when a mouse press is triggered
		virtual void OnButtonPress(Input::Buttoncode buttoncode, Input::Keymod mod);

		/// @brief called from window when a mouse release is triggered
		virtual void OnButtonRelease(Input::Buttoncode buttoncode);

		/// @brief called from window when a mouse 'scroll' is triggered
		virtual void OnMouseScroll(double xoffset, double yoffset);

		/// @brief called from window when a mouse move is triggered
		virtual void OnMouseMove(double xoffset, double yoffset);

		/// @brief called from window when dpi change is triggered
		virtual void OnDPIChange(float scale);

	public:

		/// @brief returns the curernt style colors
		ImVec4* GetStyleColors();

		/// @brief sets a style to the gui
		void SetStyle(Style style);

		/// @brief Darkish theme
		void SetDarkishTheme();

		/// @brief Dark theme
		void SetDarkTheme();

		/// @brief Light theme
		void SetLightTheme();

		/// @brief Sunset theme
		void SetSunsetTheme();

		/// @brief PurpleHaze theme
		void SetPurpleHazeTheme();

		/// @brief GoldSrc inspired style
		void SetGoldSrcTheme();

    private:

		ApplicationBase* mApp = nullptr;
		DualContainer<WidgetBase*> mWidgets;
		Library<const char*, void*> mDescriptorImages;
		Style mCurrentStyle = Classic;
    };
}