#include "Editor.h"

#include "UI/Dockspace.h"
#include "UI/EntityView.h"
#include "UI/Viewport.h"

namespace Cosmos
{
	Editor::Editor(const ApplicationBase::CreateInfo& info)
		: ApplicationBase(info)
	{

		// adding widgets
		mDockspace = new Dockspace(this);
		GetGUI()->AddWidget(mDockspace);

		mEntityView = new EntityView(this);
		GetGUI()->AddWidget(mEntityView);

		mViewport = new Viewport(this);
		GetGUI()->AddWidget(mViewport);
	}

	Editor::~Editor()
	{
		delete mViewport;
		delete mEntityView;
		delete mDockspace;
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

		evkCamera* cam = evk_get_main_camera();
		if (evk_camera_get_lock(cam))
		{
			if (keycode == Input::KEYCODE_W) { evk_camera_move(cam, evk_Camera_Direction_Forward, true); }
			if (keycode == Input::KEYCODE_S) { evk_camera_move(cam, evk_Camera_Direction_Backward, true); }
			if (keycode == Input::KEYCODE_A) { evk_camera_move(cam, evk_Camera_Direction_Left, true); }
			if (keycode == Input::KEYCODE_D) { evk_camera_move(cam, evk_Camera_Direction_Right, true); }
			if (keycode == Input::KEYCODE_LSHIFT) { evk_camera_set_speed_modifier(cam, true, 2.5f); }
		}

		if (keycode == Input::KEYCODE_Z)
		{
			if (evk_camera_get_lock(cam)) {
				GetWindow()->ToggleCursor();
				GetGUI()->ToggleCursor();
				evk_camera_set_lock(cam, false);
				return;
			}

			GetWindow()->ToggleCursor();
			GetGUI()->ToggleCursor();
			evk_camera_set_lock(cam, true);
		}
	}

	void Editor::OnKeyRelease(Input::Keycode keycode)
	{
		ApplicationBase::OnKeyRelease(keycode);

		evkCamera* cam = evk_get_main_camera();
		if (evk_camera_get_lock(cam))
		{
			if (keycode == Input::KEYCODE_W) { evk_camera_move(cam, evk_Camera_Direction_Forward, false); }
			if (keycode == Input::KEYCODE_S) { evk_camera_move(cam, evk_Camera_Direction_Backward, false); }
			if (keycode == Input::KEYCODE_A) { evk_camera_move(cam, evk_Camera_Direction_Left, false); }
			if (keycode == Input::KEYCODE_D) { evk_camera_move(cam, evk_Camera_Direction_Right, false); }
			if (keycode == Input::KEYCODE_LSHIFT) { evk_camera_set_speed_modifier(cam, false, 1.0f); }
		}
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

		evkCamera* cam = evk_get_main_camera();
		if (evk_camera_get_lock(cam))
		{
			float3 rot = { float(-yoffset), float(-xoffset), 0.0f };
			evk_camera_rotate(cam, rot);
		}
	}

	void Editor::OnDPIChange(float scale)
	{
		ApplicationBase::OnDPIChange(scale);
	}
}