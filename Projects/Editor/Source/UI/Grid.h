#pragma once

#include <Engine.h>

namespace Cosmos
{
    class Grid : public WidgetBase
    {
    public:

        /// @brief constructor
        Grid(ApplicationBase* app);

        /// @brief destructor
        virtual ~Grid();

    public:

        /// @brief updates the logic
        virtual void Update() override;

        /// @brief updates the renders
        virtual void Render() override;

    public:

        /// @brief sets the grid menu options position on the window
        void SetMenuPosition(float2 pos);

    private:

        ApplicationBase* mApp = nullptr;
        float2 mMenuPosition = {};
        float mSnappingValue = 1.0f;
        bool mSnapping = true;
        bool mVisible = true;

        evkGrid* mGrid = NULL;
    };
}