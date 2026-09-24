#include "Core/FrameStatistics.h"

#include <algorithm>
#include <numeric>

namespace Abomination::Core
{
    void FrameStatistics::AddFrame(float frameTime) noexcept
    {
        m_samples[m_nextIndex] = frameTime;

        // Move to the next slot; after the last slot go back to the first one (the "ring").
        m_nextIndex = (m_nextIndex + 1) % MaxSampleCount;
        m_storedSampleCount = std::min(m_storedSampleCount + 1, MaxSampleCount);
    }

    float FrameStatistics::GetAverageFrameTime() const noexcept
    {
        if (m_storedSampleCount == 0)
            return 0.0f;

        // 120 additions per call are negligible. Summing again every time is simpler and more precise than keeping
        // a running sum, which would slowly collect float rounding errors over hours of play.
        const std::span<const float> samples = GetSamples();
        const float sum = std::accumulate(samples.begin(), samples.end(), 0.0f);

        return sum / static_cast<float>(samples.size());
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
        if (m_storedSampleCount == 0)
            return 0.0f;

        const std::span<const float> samples = GetSamples();

        return *std::max_element(samples.begin(), samples.end());
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
