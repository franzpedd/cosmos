#pragma once

#include <Engine.h>

namespace Cosmos
{
	class Editor : public ApplicationBase
	{
	public:

		/// @brief constructor
		Editor(const ApplicationBase::CreateInfo& info);

		/// @brief destructor
		virtual ~Editor();

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

	};
}