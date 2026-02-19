#include "Editor.h"

#include "UI/Demo.h"

namespace Cosmos
{
	Editor::Editor(const ApplicationBase::CreateInfo& info)
		: ApplicationBase(info)
	{
		mDemo = new Demo();
		GetGUI()->AddWidget(mDemo);
	}

	Editor::~Editor()
	{
		delete mDemo;
	}

	void Editor::OnMinimize()
	{
		ApplicationBase::OnMinimize();
	}

	void Editor::OnMaximize()
	{
		ApplicationBase::OnMaximize();
	}

	void Editor::OnRestore()
	{
		ApplicationBase::OnRestore();
	}

	void Editor::OnResize(int32_t width, int32_t height)
	{
		ApplicationBase::OnResize(width, height);
	}

	void Editor::OnKeyPress(Input::Keycode keycode, Input::Keymod mod, bool held)
	{
		ApplicationBase::OnKeyPress(keycode, mod, held);
	}

	void Editor::OnKeyRelease(Input::Keycode keycode)
	{
		ApplicationBase::OnKeyRelease(keycode);
	}

	void Editor::OnButtonPress(Input::Buttoncode buttoncode, Input::Keymod mod)
	{
		ApplicationBase::OnButtonPress(buttoncode, mod);
	}

	void Editor::OnButtonRelease(Input::Buttoncode buttoncode)
	{
		ApplicationBase::OnButtonRelease(buttoncode);
	}

	void Editor::OnMouseScroll(double xoffset, double yoffset)
	{
		ApplicationBase::OnMouseScroll(xoffset, yoffset);
	}

	void Editor::OnMouseMove(double xoffset, double yoffset)
	{
		ApplicationBase::OnMouseMove(xoffset, yoffset);
	}

	void Editor::OnDPIChange(float scale)
	{
		ApplicationBase::OnDPIChange(scale);
	}
}