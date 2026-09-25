#include "Core/FrameStatistics.h"

#include <algorithm>

namespace Abomination::Core
{
    void FrameStatistics::AddFrame(float frameTime) noexcept
    {
        // The graph: write into the ring buffer, then move to the next slot; after the last slot go back to the first one.
        m_samples[m_nextIndex] = frameTime;
        m_nextIndex = (m_nextIndex + 1) % MaxSampleCount;
        m_storedSampleCount = std::min(m_storedSampleCount + 1, MaxSampleCount);

        // The numbers to read: the frame joins the current interval.
        m_intervalTime += frameTime;
        ++m_intervalFrameCount;
        m_intervalLongestFrameTime = std::max(m_intervalLongestFrameTime, frameTime);

        if (m_intervalTime < ReportInterval)
            return;

        // The interval is over: its result becomes the new reported values, and a new interval starts empty.
        // The interval can be a little longer than ReportInterval (it ends on a frame boundary); the average is still
        // exact, because it divides the real total time by the real number of frames.
        m_reportedAverageFrameTime = m_intervalTime / static_cast<float>(m_intervalFrameCount);
        m_reportedLongestFrameTime = m_intervalLongestFrameTime;
        m_wasFirstIntervalFinished = true;

        m_intervalTime = 0.0f;
        m_intervalFrameCount = 0;
        m_intervalLongestFrameTime = 0.0f;
    }

    float FrameStatistics::GetAverageFrameTime() const noexcept
    {
        if (m_wasFirstIntervalFinished)
            return m_reportedAverageFrameTime;

        if (m_intervalFrameCount == 0)
            return 0.0f;

        return m_intervalTime / static_cast<float>(m_intervalFrameCount);
    }

    float FrameStatistics::GetAverageFramesPerSecond() const noexcept
    {
        const float averageFrameTime = GetAverageFrameTime();
        if (averageFrameTime <= 0.0f)
            return 0.0f;

        return 1.0f / averageFrameTime;
    }

    float FrameStatistics::GetLongestFrameTime() const noexcept
    {
        if (m_wasFirstIntervalFinished)
            return m_reportedLongestFrameTime;

        return m_intervalLongestFrameTime;
    }

    std::span<const float> FrameStatistics::GetSamples() const noexcept
    {
        // Until the buffer is full, only its beginning contains real samples.
        return std::span<const float>(m_samples.data(), m_storedSampleCount);
    }

    std::size_t FrameStatistics::GetOldestSampleIndex() const noexcept
    {
        // Before the buffer is full, samples were written from index 0, so the oldest one is there.
        // After that, the slot that will be overwritten next is the one holding the oldest sample.
        if (m_storedSampleCount < MaxSampleCount)
            return 0;

        return m_nextIndex;
    }
}
