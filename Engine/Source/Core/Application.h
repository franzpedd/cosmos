#pragma once

#include "Core/Defines.h"
#include "Core/Input.h"

/// @brief forward declarations
namespace Cosmos { class GUI; }
namespace Cosmos { class Renderer; }
namespace Cosmos { class Window; }
namespace Cosmos { class World; }

namespace Cosmos
{
	class COSMOS_API ApplicationBase
	{
	public:

		/// @brief initialization variables
		struct CreateInfo
		{
			/// @brief this is the application name, used in the window as well as internally by the renderer
			const char* appName = nullptr;

			/// @brief tells the renderer we're going to create a customized viewport that is pottentially different than the one provided by cren (wich covers the entire window)
			bool customViewport = false;

			/// @brief tells the renderer we're going to need validations (not a good idea on an physical android device, only emulated)
			bool validations = true;

			/// @brief tells the window manager that the application will cover the entire window area
			bool fullscreen = false;

			/// @brief request vertical syncronization
			bool vsync = false;

			/// @brief tells the window manager it's width size, this can latter be changed
			int32_t width = 1366;

			/// @brief tells the window manager it's height size, this can latter be changed
			int32_t height = 768;

			/// @brief tells the engine and renderer where the assets directory is located
			const char* assetsPath = nullptr;
		};

	public:

		/// @brief constructor
		ApplicationBase(const CreateInfo& info);

		/// @brief destructor
		virtual ~ApplicationBase();

		/// @brief returns the gui object
		inline GUI* GetGUI() const { return mGUI; }

		/// @brief returns the renderer object
		inline Renderer* GetRenderer() const { return mRenderer; }

		/// @brief returns the window object
		inline Window* GetWindow() const { return mWindow; }

		/// @brief returns the world object
		inline World* GetWorld() const { return mWorld; }

		/// @brief returns the timestep
		inline float GetTimestep() const { return mTimestep; }

		/// @brief returns the average frames per second
		inline float GetAverageFPS() const { return mAverageFPS; }
		

	public:

		/// @brief initializes the loop/run process
		void Run();

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
		GUI* mGUI;
		Renderer* mRenderer;
		Window* mWindow;
		World* mWorld;

		float mAverageFPS = 0.0f;
		float mTimestep = 0.0f;
		const char* mAssetsPath = nullptr;
	};
}