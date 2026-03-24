#pragma once

#include <Engine.h>

namespace Cosmos
{
    class Dockspace : public WidgetBase
    {
    public:

        /// @brief constructor
        Dockspace(ApplicationBase* app);

        /// @brief destructor
        virtual ~Dockspace();

    public:

        /// @brief updates the logic
        virtual void Update() override;

        /// @brief updates the renders
        virtual void Render() override;

    private:
        
        ApplicationBase* mApp;
    };
}