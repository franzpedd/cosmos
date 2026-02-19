#include "Window.h"

#include "Core/Application.h"
#include "Core/Logger.h"
#include "UI/Images.h"
#include <SDL3/SDL.h>
#include <evk.h>

namespace Cosmos
{
	Window::Window(ApplicationBase* app, const char* title, int32_t width, int32_t height, bool fullscreen)
		: mApp(app), mTitle(title), mWidth(width), mHeight(height)
	{
		if (SDL_Init(SDL_INIT_VIDEO) == false) {
			LOG_TO_TERMINAL(Logger::Error, "SDL could not be initialized, more info: %s", SDL_GetError());
			return;
		}

		SDL_WindowFlags flags = fullscreen == true ? SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_FULLSCREEN
			: SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

		mNativeWindow = SDL_CreateWindow(title, width, height, flags);
		if (!mNativeWindow) {
			LOG_TO_TERMINAL(Logger::Error, "Window could not be created, more info: %s", SDL_GetError());
			SDL_Quit();
			return;
		}

		#if defined(__APPLE__)
		mMetalView = SDL_Metal_CreateView(mNativeWindow);
		#endif

		LOG_TO_TERMINAL(Logger::Todo, "Implement mobile touch events");
		LOG_TO_TERMINAL(Logger::Todo, "Implement imgui events");
		LOG_TO_TERMINAL(Logger::Todo, "Implement icon loading");

		int32_t iconWidth, iconHeight, iconChannels;
		unsigned char* icon = evk_stb_load_from_memory(incorelogo_data, incorelogo_size, &iconWidth, &iconHeight, &iconChannels, 4);
		if (!icon) {
			LOG_TO_TERMINAL(Logger::Error, "Failed to load window icon. Reason: %s", evk_stb_failure_reason());
		}

		mIcon = SDL_CreateSurfaceFrom(iconWidth, iconHeight, SDL_PIXELFORMAT_RGBA32, icon, iconWidth * 4);
		if (!SDL_SetWindowIcon(mNativeWindow, mIcon)) {
			LOG_TO_TERMINAL(Logger::Error, "Error while setting up the window icon. Reason: %s", SDL_GetError());
		}

		evk_stb_free(icon);
	}

	Window::~Window()
	{
		#if defined(__APPLE__)
		SDL_Metal_DestroyView(mMetalView);
		#endif
		SDL_DestroySurface(mIcon);
		SDL_DestroyWindow((SDL_Window*)mNativeWindow);
		SDL_Quit();
	}

	void Window::Update()
	{
		SDL_Event event = { 0 };

		while (SDL_PollEvent(&event))
		{
			//ImGui_ImplSDL3_ProcessEvent(&event);

			switch (event.type)
			{
				case SDL_EVENT_QUIT:
				{
					mShouldClose = true;
					break;
				}

				case SDL_EVENT_KEY_DOWN:
				{
					mApp->OnKeyPress((Input::Keycode)event.key.scancode, (Input::Keymod)event.key.mod, false);
					break;
				}

				case SDL_EVENT_KEY_UP:
				{
					mApp->OnKeyRelease((Input::Keycode)event.key.scancode);
					break;
				}

				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				{
					mApp->OnButtonPress((Input::Buttoncode)event.button.button, Input::Keymod::KEYMOD_NONE);
					break;
				}

				case SDL_EVENT_MOUSE_BUTTON_UP:
				{
					mApp->OnButtonRelease((Input::Buttoncode)event.button.button);
					break;
				}

				case SDL_EVENT_MOUSE_WHEEL:
				{
					mApp->OnMouseScroll((double)event.wheel.x, -event.wheel.y);
					break;
				}

				case SDL_EVENT_MOUSE_MOTION:
				{
					mApp->OnMouseMove((double)event.motion.xrel, (double)event.motion.yrel);
					break;
				}

				case SDL_EVENT_WINDOW_RESIZED:
				{
					mWidth = event.window.data1;
					mHeight = event.window.data2;
					mApp->OnResize(event.window.data1, event.window.data2);
					break;
				}

				case SDL_EVENT_WINDOW_MINIMIZED:
				{
					mMinimized = true;
					mApp->OnMinimize();
					break;
				}

				case SDL_EVENT_WINDOW_MAXIMIZED:
				{
					mMinimized = false;
					mApp->OnMaximize();
					break;
				}

				case SDL_EVENT_WINDOW_RESTORED:
				{
					mMinimized = false;
					mApp->OnRestore();
					break;
				}

				case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
				{
					mApp->OnDPIChange(SDL_GetWindowDisplayScale(mNativeWindow));
					break;
				}

				default:
				{
					break;
				}
			}
		}
	}

	void Window::Quit()
	{
		mShouldClose = true;
	}

	bool Window::IsKeyDown(Input::Keycode keycode)
	{
		const bool* keyboardState = SDL_GetKeyboardState(NULL);
		return keyboardState[keycode] == 1;
	}

	bool Window::IsButtonDown(Input::Buttoncode button)
	{
		Uint32 mouseState = SDL_GetMouseState(NULL, NULL);

		switch (button)
		{
			case Input::Buttoncode::BUTTON_LEFT: return mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
			case Input::Buttoncode::BUTTON_MIDDLE: return mouseState & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE);
			case Input::Buttoncode::BUTTON_RIGHT: return mouseState & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
			case Input::Buttoncode::BUTTON_X1: return mouseState & SDL_BUTTON_MASK(SDL_BUTTON_X1);
			case Input::Buttoncode::BUTTON_X2: return mouseState & SDL_BUTTON_MASK(SDL_BUTTON_X2);
		}

		return false;
	}

	void Window::ToggleCursor()
	{
		mCursorVisible = !mCursorVisible;

		SDL_SetWindowRelativeMouseMode(mNativeWindow, mCursorVisible);

		if (!mCursorVisible) {
			int w, h;
			SDL_GetWindowSize(mNativeWindow, &w, &h);
			SDL_WarpMouseInWindow(mNativeWindow, (float)(w / 2), (float)(h / 2));
		}
	}

	float2 Window::GeSize()
	{
		int width, height;
		SDL_GetWindowSize(mNativeWindow, &width, &height);

		return { (float)width, (float)height };
	}

	float2 Window::GetPos()
	{
		int x, y;

		SDL_GetWindowPosition(mNativeWindow, &x, &y);
		return { (float)x, (float)y };
	}

	float2 Window::GetCursorPos()
	{
		float2 pos = { 0.0f, 0.0f };
		SDL_GetGlobalMouseState(&pos.xy.x, &pos.xy.y);

		return pos;
	}

	float Window::GetRefreshRate()
	{
		const float DEFAULT_REFRESH_RATE = 60.0f;

		SDL_DisplayID display = SDL_GetDisplayForWindow(mNativeWindow);
		if (display == 0) {
			LOG_TO_TERMINAL(Logger::Error, "Failed to get display for window: %s", SDL_GetError());
			return DEFAULT_REFRESH_RATE;
		}

		const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
		if (!mode) {
			LOG_TO_TERMINAL(Logger::Error, "Failed to get current display mode: %s", SDL_GetError());
			return DEFAULT_REFRESH_RATE;
		}

		if (mode->refresh_rate > 0) {
			LOG_TO_TERMINAL(Logger::Info, "Detected refresh rate: %f Hz", mode->refresh_rate);
			return mode->refresh_rate;
		}

		LOG_TO_TERMINAL(Logger::Warn, "Display reported invalid refresh rate: %f Hz", mode->refresh_rate);
		return DEFAULT_REFRESH_RATE;
	}

	uint64_t Window::GetTimer()
	{
		return SDL_GetPerformanceCounter();
	}

	uint64_t Window::GetTimerFrequency()
	{
		return SDL_GetPerformanceFrequency();
	}

	#if defined(_WIN32)
	void* Window::Win32_GetHWND()
	{
		return SDL_GetPointerProperty(SDL_GetWindowProperties(mNativeWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	}
	#endif

	#if defined(__APPLE__)
	void* Window::Apple_GetCAMetalLayer()
	{
		return SDL_Metal_GetLayer(mMetalView);
	}
	#endif

	#if defined(__ANDROID__)
	void* Window::Android_GetANativeWindow()
	{
		return SDL_GetPointerProperty(SDL_GetWindowProperties(mNativeWindow), SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, NULL);
	}
	#endif

	#if defined(EVK_LINUX_USE_XLIB)
	void* Window::XLIB_GetDisplay()
	{
		return SDL_GetPointerProperty(SDL_GetWindowProperties(mNativeWindow), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
	}

	unsigned long Window::XLIB_GetWindow()
	{
		return SDL_GetPointerProperty(SDL_GetWindowProperties(mNativeWindow), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
	}
	#endif

	#if defined(EVK_LINUX_USE_XCB)
	void* Window::XCB_GetConnection()
	{
		// this will throw errors because of missing includes, if anyone needs xcb support contact me but fell free to fix it yourself and send patches
		Display* display = (Display*)SDL_GetPointerProperty(SDL_GetWindowProperties(mNativeWindow), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
		XGetXCBConnectionFunc XGetXCBConnection = (XGetXCBConnectionFunc)dlsym(RTLD_DEFAULT, "XGetXCBConnection");
		return XGetXCBConnection(display);
	}

	uint32_t Window::XCB_GetWindow()
	{
		return (uint32_t)SDL_GetPointerProperty(SDL_GetWindowProperties(mNativeWindow), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0); // same as X11 but as uint32_t
	}
	#endif

	#if defined(__linux__) && !defined(__ANDROID__) && !defined(EVK_LINUX_USE_XLIB) && !defined(EVK_LINUX_USE_XCB)
	void* Window::Wayland_GetDisplay()
	{
		return SDL_GetPointerProperty(SDL_GetWindowProperties(mNativeWindow), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
	}

	void* Window::Wayland_GetSurface()
	{
		return SDL_GetPointerProperty(SDL_GetWindowProperties(mNativeWindow), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
	}
	#endif
}