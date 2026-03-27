#pragma once

#include "Defines.h"
#include "Util/ID.h"

/// @brief forward declarations
namespace Cosmos { class ApplicationBase; }

namespace Cosmos
{
    class COSMOS_API Renderer
    {
    public:

        /// @brief constructor
        Renderer(ApplicationBase* app, const char* appName, uint32_t width, uint32_t height, bool viewport, bool vsync);

        /// @brief destructor
        ~Renderer();

        /// @brief returns the renderer version
        inline uint32_t GetAPIVersion() const { return mAPIVersion; }

        /// @brief returns if vsync is enabled/disabled
        inline bool GetVSync() const { return mVSync; }

        /// @brief returns a reference to the id generator
        inline IDGen& GetIDGenRef() { return mIDGen; }

    public:

        /// @brief begins the updating of the frame
        void Update(float timestep);

        /// @brief begins the rendering of the frame
        void Render(float timestep);

        /// @brief called when it's time to draw individual objects
        void OnRenderCallback(float timestep);

        /// @brief called when it's time to draw ui individual objects
        void OnRenderUICallback(void* rawCommandBuffer);

        /// @brief hints the renderer minimization
        void OnMinimize();

        /// @brief hints the renderer maximization
        void OnMaximize();

        /// @brief hints the renderer size restore
        void OnRestore();

        /// @brief resizes the renderer
        void OnResize(int32_t width, int32_t height);

    private:
    
        ApplicationBase* mApp = nullptr;
        uint32_t mAPIVersion = 0;
        bool mVSync = false;
        IDGen mIDGen;
    };
}