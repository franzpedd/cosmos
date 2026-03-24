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


    /// @brief returns a formated unique compilation version
    COSMOS_API inline int64_t GetCompilationVersion()
    {
        const char* date = __DATE__;
        const char* time = __TIME__;

        const char* months[] = {
            "Jan","Feb","Mar","Apr","May","Jun",
            "Jul","Aug","Sep","Oct","Nov","Dec"
        };

        int month = 0;
        for (int i = 0; i < 12; i++) {
            if (strncmp(date, months[i], 3) == 0) {
                month = i + 1;
                break;
            }
        }

        int day = atoi(date + 4);
        int year = atoi(date + 7);

        int hour = (time[0] - '0') * 10 + (time[1] - '0');
        int minute = (time[3] - '0') * 10 + (time[4] - '0');

        // YYYYMMDDHHMM (requires 64-bit)
        return (int64_t)year * 100000000LL + (int64_t)month * 1000000LL + (int64_t)day * 10000LL + (int64_t)hour * 100LL + (int64_t)minute;
    }

    /// @brief formats the compilation version into a string
    COSMOS_API inline void FormatCompilationVersion(int64_t version, char* out, size_t outSize)
    {
        int64_t year = version / 100000000;
        int64_t month = (version / 1000000) % 100;
        int64_t day = (version / 10000) % 100;
        int64_t hour = (version / 100) % 100;
        int64_t minute = version % 100;

        snprintf(out, outSize,"%04lld-%02lld-%02lld %02lld:%02lld",(long long)year, (long long)month, (long long)day, (long long)hour, (long long)minute);
    }
}