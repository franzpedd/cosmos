#pragma once

#include <Engine.h>

namespace Cosmos
{
    class Demo : public WidgetBase
    {
    public:

        /// @brief constructor
        Demo();

        /// @brief destructor
        virtual ~Demo();

    public:

        /// @brief updates the logic
        virtual void Update() override;

        /// @brief updates the renders
        virtual void Render() override;

    };
}