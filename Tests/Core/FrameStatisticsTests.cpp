#include "Core/FrameStatistics.h"

#include <gtest/gtest.h>

#include <cstddef>

namespace Abomination::Core
{
    // Averages are sums of floats, so they may differ from the exact value in the last digits.
    constexpr float Tolerance = 1e-6f;

    TEST(FrameStatistics, EverythingIsZeroWithoutFrames)
    {
        const FrameStatistics statistics;

        EXPECT_FLOAT_EQ(statistics.GetAverageFrameTime(), 0.0f);
        EXPECT_FLOAT_EQ(statistics.GetAverageFramesPerSecond(), 0.0f);
        EXPECT_FLOAT_EQ(statistics.GetLongestFrameTime(), 0.0f);
        EXPECT_TRUE(statistics.GetSamples().empty());
    }

    TEST(FrameStatistics, AveragesStoredFrames)
    {
        FrameStatistics statistics;

        statistics.AddFrame(0.010f);
        statistics.AddFrame(0.020f);
        statistics.AddFrame(0.030f);

        EXPECT_NEAR(statistics.GetAverageFrameTime(), 0.020f, Tolerance);
        EXPECT_NEAR(statistics.GetAverageFramesPerSecond(), 50.0f, 0.001f);
    }

    TEST(FrameStatistics, FindsLongestFrame)
    {
        FrameStatistics statistics;

        statistics.AddFrame(0.016f);
        statistics.AddFrame(0.080f);
        statistics.AddFrame(0.017f);

        EXPECT_FLOAT_EQ(statistics.GetLongestFrameTime(), 0.080f);
    }

    TEST(FrameStatistics, OldestSampleIsFirstUntilBufferIsFull)
    {
        FrameStatistics statistics;

        statistics.AddFrame(0.016f);
        statistics.AddFrame(0.017f);

        EXPECT_EQ(statistics.GetSamples().size(), 2u);
        EXPECT_EQ(statistics.GetOldestSampleIndex(), 0u);
    }

    TEST(FrameStatistics, NewFramesOverwriteOldestWhenBufferIsFull)
    {
        FrameStatistics statistics;

        // One very long frame first, then enough short frames to fill the buffer and push the long one out.
        statistics.AddFrame(1.0f);
        for (std::size_t frame = 0; frame < FrameStatistics::MaxSampleCount + 1; ++frame)
            statistics.AddFrame(0.010f);

        EXPECT_EQ(statistics.GetSamples().size(), FrameStatistics::MaxSampleCount);
        EXPECT_EQ(statistics.GetOldestSampleIndex(), 2u);
        EXPECT_FLOAT_EQ(statistics.GetLongestFrameTime(), 0.010f);
        EXPECT_NEAR(statistics.GetAverageFrameTime(), 0.010f, Tolerance);
    }
}
