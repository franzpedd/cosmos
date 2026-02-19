#pragma once

#include "Core/Defines.h"
#include <chrono>
#include <thread>

namespace Cosmos
{
	/// @brief type aliases
	using Clock = std::chrono::steady_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	using Duration = std::chrono::duration<double>;

    class COSMOS_API FrameLimiter
    {
    public:

        /// @brief constructor
        FrameLimiter(float targetFPS)
        {
            mTargetDuration = std::chrono::nanoseconds(static_cast<int64_t>(1e9 / targetFPS));
            mFrameStart = std::chrono::steady_clock::now();
        }

        /// @brief stalls the cpu if necessary
        inline void Wait()
        {
            auto now = std::chrono::steady_clock::now();

            // calculate next frame time
            mFrameStart += mTargetDuration;

            // if we're behind, catch up by setting frame start to now + target duration
            if (mFrameStart < now) {
                mFrameStart = now + mTargetDuration;
            }

            // hybrid approach: sleep + busy-wait for precision
            auto remaining = mFrameStart - now;

            if (remaining > std::chrono::microseconds(2000)) {
                std::this_thread::sleep_for(remaining - std::chrono::microseconds(1500));
            }

            while (std::chrono::steady_clock::now() < mFrameStart) {
                std::this_thread::yield();
            }
        }

    private:
        std::chrono::steady_clock::time_point mFrameStart;
        std::chrono::nanoseconds mTargetDuration;
    };
}