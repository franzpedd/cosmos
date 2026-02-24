#include "Application.h"

#include "Core/Logger.h"
#include "Core/Renderer.h"
#include "Core/Window.h"
#include "Scene/World.h"
#include "UI/GUI.h"
#include "Util/Clock.h"

namespace Cosmos
{
	ApplicationBase::ApplicationBase(const CreateInfo& info)
        : mAssetsPath(info.assetsPath)
	{
        mWindow = new Window(this, info.appName, info.width, info.height, info.fullscreen);
        mRenderer = new Renderer(this, info.appName, info.width, info.height, info.customViewport, info.vsync);
        mGUI = new GUI(this);
        mWorld = new World(mRenderer, "Default World");
	}

    ApplicationBase::~ApplicationBase()
    {
        delete mWorld;
        delete mGUI;
        delete mRenderer;
        delete mWindow;
    }

    void ApplicationBase::Run()
    {
        // time tracking
        TimePoint previousTime = Clock::now();
        float accumulator = 0.0f;
        const float FIXED_TIMESTEP = 1.0f / 60.0f;
        const int MAX_UPDATES = 10;
        
        // fps tracking
        int frameCount = 0;
        float fpsAccumulator = 0.0f;
        mAverageFPS = 0.0f;
        
        // fps limiter
        float targetFPS = (float)(mWindow->GetRefreshRate() * 2.0);
        FrameLimiter frameLimiter(targetFPS);
        
        while (!mWindow->ShouldClose())
        {
            TimePoint frameStart = Clock::now();
        
            // deltatime
            TimePoint currentTime = Clock::now();
            mTimestep = (float)(std::chrono::duration_cast<Duration>(currentTime - previousTime).count());
            previousTime = currentTime;
        
            // cap maximum timestep to prevent spiral of death
            const float MAX_TIMESTEP = 0.05f;
            if (mTimestep > MAX_TIMESTEP) mTimestep = MAX_TIMESTEP;
        
            // update window events
            mWindow->Update();
            mGUI->Update();
        
            // accumulate time for fixed updates
            accumulator += mTimestep;
        
            // fps tracking
            frameCount++;
            fpsAccumulator += mTimestep;
            if (fpsAccumulator >= 1.0f) {
                mAverageFPS = (float)(frameCount);
                frameCount = 0;
                fpsAccumulator -= 1.0f;
            }
        
            // cap accumulator to prevent catch-up spiral
            if (accumulator > FIXED_TIMESTEP * MAX_UPDATES) {
                accumulator = FIXED_TIMESTEP * MAX_UPDATES;
            }
        
            // fixed timestep updates
            int updateCount = 0;
            while (accumulator >= FIXED_TIMESTEP && updateCount < MAX_UPDATES) {
                mRenderer->Update(FIXED_TIMESTEP);
                accumulator -= FIXED_TIMESTEP;
                updateCount++;
            }
        
            // updates with interpolation
            float alpha = accumulator / FIXED_TIMESTEP;
            mRenderer->Render(alpha);
        
            // use frame limiter instead of manual sleep
            if (!mRenderer->GetVSync()) {
                frameLimiter.Wait();
            }
        }
	}

    void ApplicationBase::OnMinimize()
    {
        mRenderer->OnMinimize();
        mGUI->OnMinimize();
    }

    void ApplicationBase::OnMaximize()
    {
        mRenderer->OnMaximize();
        mGUI->OnMaximize();
    }

    void ApplicationBase::OnRestore()
    {
        mRenderer->OnRestore();
        mGUI->OnRestore();
    }

    void ApplicationBase::OnResize(int32_t width, int32_t height)
    {
        mRenderer->OnResize(width, height);
        mGUI->OnResize(width, height);
    }

    void ApplicationBase::OnKeyPress(Input::Keycode keycode, Input::Keymod mod, bool held)
    {
        mGUI->OnKeyPress(keycode, mod, held);
    }

    void ApplicationBase::OnKeyRelease(Input::Keycode keycode)
    {
        mGUI->OnKeyRelease(keycode);
    }

    void ApplicationBase::OnButtonPress(Input::Buttoncode buttoncode, Input::Keymod mod)
    {
        mGUI->OnButtonPress(buttoncode, mod);
    }

    void ApplicationBase::OnButtonRelease(Input::Buttoncode buttoncode)
    {
        mGUI->OnButtonRelease(buttoncode);
    }

    void ApplicationBase::OnMouseScroll(double xoffset, double yoffset)
    {
        mGUI->OnMouseScroll(xoffset, yoffset);
    }

    void ApplicationBase::OnMouseMove(double xoffset, double yoffset)
    {
        mGUI->OnMouseMove(xoffset, yoffset);
    }

    void ApplicationBase::OnDPIChange(float scale)
    {
        mGUI->OnDPIChange(scale);
    }
}