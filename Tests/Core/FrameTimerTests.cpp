#include "Core/FrameTimer.h"

#include <gtest/gtest.h>

#include <chrono>

namespace Abomination::Core
{
    using namespace std::chrono_literals;

    TEST(FrameTimer, DeltaTimeIsZeroBeforeFirstFrame)
    {
        const FrameTimer timer(TimePoint{});

        EXPECT_FLOAT_EQ(timer.GetDeltaTime(), 0.0f);
        EXPECT_DOUBLE_EQ(timer.GetTotalTime(), 0.0);
    }

    TEST(FrameTimer, DeltaTimeIsTimeSincePreviousFrame)
    {
        const TimePoint start{};
        FrameTimer timer(start);

        timer.StartFrame(start + 16ms);
        timer.StartFrame(start + 50ms);

        EXPECT_FLOAT_EQ(timer.GetDeltaTime(), 0.034f);
    }

    TEST(FrameTimer, TotalTimeIsSumOfFrames)
    {
        const TimePoint start{};
        FrameTimer timer(start);

        timer.StartFrame(start + 100ms);
        timer.StartFrame(start + 300ms);
        timer.StartFrame(start + 350ms);

        EXPECT_DOUBLE_EQ(timer.GetTotalTime(), 0.35);
    }

    TEST(FrameTimer, LongFrameIsClampedToMaxDeltaTime)
    {
        const TimePoint start{};
        FrameTimer timer(start);

        timer.StartFrame(start + 5s);

        EXPECT_FLOAT_EQ(timer.GetDeltaTime(), static_cast<float>(FrameTimer::MaxDeltaTime));
        EXPECT_DOUBLE_EQ(timer.GetTotalTime(), FrameTimer::MaxDeltaTime);
    }
}
