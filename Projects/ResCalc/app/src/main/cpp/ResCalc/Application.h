#ifndef PROJECT_ANDROID_APPLICATION_H
#define PROJECT_ANDROID_APPLICATION_H

#include <Engine.h>

/// @brief forward declarations
namespace Cosmos { class ResistorCalcWidget; }

namespace Cosmos
{
    class Application : public Cosmos::ApplicationBase
    {
    public:

        /// @brief constructor
        Application(const CreateInfo& ci);

        /// @brief destructor
        virtual ~Application() = default;

    public:

    private:
        ResistorCalcWidget* mResistorCalcWidget = nullptr;
    };
}

#endif //PROJECT_ANDROID_APPLICATION_H