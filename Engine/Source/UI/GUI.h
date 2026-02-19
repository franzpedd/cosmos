#pragma once

#include "Core/Defines.h"
#include "Core/Input.h"
#include "Util/Container.h"

/// @brief forward declarations
namespace Cosmos { class ApplicationBase; }
namespace Cosmos { class WidgetBase; }

namespace Cosmos
{
    class COSMOS_API GUI
    {
    public:

		/// @brief constructor
		GUI(ApplicationBase* app);

		/// @brief destructor
		virtual ~GUI();

		/// @brief returns the imgui context
		static void* GetImGuiContext();

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

    private:

		ApplicationBase* mApp = nullptr;
		bool mCursorVisible = true;
		DualContainer<WidgetBase*> mWidgets;
    };
}