#include "Renderer.h"

#include "Core/Application.h"
#include "Core/Logger.h"
#include "Core/Window.h"
#include "Scene/World.h"
#include "UI/GUI.h"

#include <EVK.h>

namespace Cosmos
{
	Renderer::Renderer(ApplicationBase* app, const char* appName, uint32_t width, uint32_t height, bool viewport, bool vsync, bool validations)
		: mApp(app)
	{
        evkCreateInfo info = { 0 };
        info.appName = appName;
		info.engineName = "EVK";
		info.engineVersion = EVK_MAKE_VERSION(0, 0, 1, 0);
        info.appVersion = EVK_MAKE_VERSION(0, 1, 0, 0);
        info.width = width;
        info.height = height;
		info.MSAA = evk_Msaa_X4;
		info.vsync = vsync;
        info.viewport = viewport;
		info.validations = validations;

        // platform specific window
		#ifdef _WIN32
		info.window.window = (HWND)mApp->GetWindow()->Win32_GetHWND();
		#elif defined(__APPLE__)
		info.window.layer = (CAMetalLayer*)mApp->GetWindow()->Apple_GetCAMetalLayer();
		#elif defined(__ANDROID__)
		info.window.window = (ANativeWindow*)mApp->GetWindow()->Android_GetANativeWindow();
		#elif defined(EVK_LINUX_USE_XLIB)
		info.window.display = (Display*)mApp->GetWindow()->XLIB_GetDisplay();
		info.window.window = (Window)mApp->GetWindow()->XLIB_GetWindow();
		#elif defined(EVK_LINUX_USE_XCB)
		info.window.connection = (xcb_connection_t*)mApp->GetWindow()->XCB_GetConnection();
		info.window.window = (xcb_window_t)mApp->GetWindow()->XCB_GetWindow();
		#elif defined(__linux__) && !defined(__ANDROID__) && !defined(EVK_LINUX_USE_XLIB) && !defined(EVK_LINUX_USE_XCB)
		info.window.display = (wl_display*)mApp->GetWindow()->Wayland_GetDisplay();
		info.window.surface = (wl_surface*)mApp->GetWindow()->Wayland_GetSurface();
		#endif

		LOG_ASSERT(evk_init(&info) == evk_Success, "Failed to create EVK Renderer");

		// set window pointer and callbacks
		evk_set_user_pointer(this);
		
		evk_set_render_callback([](evkContext* context, float timestep)
			{
				Renderer& renderer = *(Renderer*)evk_get_user_pointer();
				renderer.OnRenderCallback(timestep);
			});

		evk_set_renderui_callback([](evkContext* context, void* rawCommandBuffer)
			{
				Renderer& renderer = *(Renderer*)evk_get_user_pointer();
				renderer.OnRenderUICallback(rawCommandBuffer);
			});

		// config variables
		mAPIVersion = info.engineVersion;
		mVSync = info.vsync;
	}

	Renderer::~Renderer()
	{
		evk_shutdown();
	}

	void Renderer::Update(float timestep)
	{
		evk_update(timestep);
	}

	void Renderer::Render(float timestep)
	{
		evk_render(timestep);
	}

	void Renderer::OnRenderCallback(float timestep)
	{
		mApp->GetGUI()->Render();
		mApp->GetWorld()->Render(timestep);
	}

	void Renderer::OnRenderUICallback(void* rawCommandBuffer)
	{
		mApp->GetGUI()->DrawRawData(rawCommandBuffer);
	}

	void Renderer::OnMinimize()
	{
		evk_minimize();
	}

	void Renderer::OnMaximize()
	{
		LOG_TO_TERMINAL(Logger::Todo, "Implement on EVK's side? At least the aspect-ratio");
	}

	void Renderer::OnRestore()
	{
		evk_restore();
	}

	void Renderer::OnResize(int32_t width, int32_t height)
	{
		float2 size = {};
		size.xy.x = (float)width;
		size.xy.y = (float)height;
		evk_resize(size);
	}
}