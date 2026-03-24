#pragma once

#include "Core/Defines.h"
#include "Core/Input.h"
#include <EVK.h>

/// @brief forward declarations
struct SDL_Window;
struct SDL_Surface;
#if defined(__APPLE__)
struct SDL_MetalView;
#endif
namespace Cosmos { class ApplicationBase; }

namespace Cosmos
{
	class COSMOS_API Window
	{
	public:

		/// @brief constructor
		Window(ApplicationBase* app, const char* title, int32_t width, int32_t height, bool fullscreen);

		/// @brief destructor
		~Window();

		/// @brief returns the native window object
		inline SDL_Window* GetNativeWindow() { return mNativeWindow; }

		/// @brief returns the window's current width
		inline int32_t GetWidth() const { return mWidth; }

		/// @brief returns the window's current height
		inline int32_t GetHeight() const { return mHeight; }

		/// @brief returns if the window is currently minimized
		inline bool Minimized() const { return mMinimized; }

		/// @brief returns if the window should be closed (a request was issued)
		inline bool ShouldClose() const { return mShouldClose; }

	public:

		/// @brief process all pending window events
		void Update();

		/// @brief signals the window quitting
		void Quit();

		/// @brief returns if a given key is currently pressed
		bool IsKeyDown(Input::Keycode keycode);

		/// @brief returns if a given mouse button is currently pressed
		bool IsButtonDown(Input::Buttoncode button);

		/// @brief hides/unhides the cursor
		void ToggleCursor();

	public:

		/// @brief returns the window's size
		float2 GeSize();
		
		/// @brief returns the window's pivot position
		float2 GetPos();

		/// @brief returns the cursor's position
		float2 GetCursorPos();

		/// @brief returns the refresh rate for the monitor the window currently is
		float GetRefreshRate();

		/// @brief returns the internal clock timer
		uint64_t GetTimer();

		/// @brief returns the internal clock frequency
		uint64_t GetTimerFrequency();

	public:

		#if defined(_WIN32)
		/// @brief returns win32 HWND for the window
		void* Win32_GetHWND();
		#endif

		#if defined(__APPLE__)
		/// @brief returns apple CAMetalLayer object
		void* Apple_GetCAMetalLayer();
		#endif

		#if defined(__ANDROID__)
		/// @brief returns android ANativeWindow object
		void* Android_GetANativeWindow();
		#endif

		#if defined(EVK_LINUX_USE_XLIB)
		/// @brief returns xlib Display object
		void* XLIB_GetDisplay();

		/// @brief returns the xlib window number
		unsigned long XLIB_GetWindow();
		#endif

		#if defined(EVK_LINUX_USE_XCB)
		/// @brief returns the xcb xcb_connection_t object
		void* XCB_GetConnection();

		/// @brief returns the xcb xcb_window_t number
		uint32_t XCB_GetWindow();
		#endif

		#if defined(__linux__) && !defined(__ANDROID__) && !defined(EVK_LINUX_USE_XLIB) && !defined(EVK_LINUX_USE_XCB)
		/// @brief returns wayland wl_display object
		void* Wayland_GetDisplay();

		/// @brief returns wayland wl_surface object
		void* Wayland_GetSurface();
		#endif

	private:
		ApplicationBase* mApp = nullptr;
		SDL_Surface* mIcon = nullptr;
		SDL_Window* mNativeWindow = nullptr;
		#if defined(__APPLE__)
		SDL_MetalView mMetalView = nullptr;
		#endif

		const char* mTitle = nullptr;
		int32_t mWidth = 1920;
		int32_t mHeight = 1080;
		bool mMinimized = false;
		bool mShouldClose = false;
		bool mCursorVisible = true;
		double mLastMousePosX = 0.0;
		double mLastMousePosY = 0.0;
	};
}