#pragma once

#include <array>
#include <cstddef>
#include <span>

namespace Abomination::Core
{
    // Remembers the durations of the last MaxSampleCount frames and calculates statistics over them:
    // average frame time, average FPS and the longest frame.
    //
    // The samples are stored in a ring buffer: a fixed array where every new frame overwrites the oldest one.
    // Once the array is full, the next sample goes to index 0 again, then 1, and so on, so no memory is ever moved.
    //
    //   after 5 frames (capacity 4):   [ f5 | f2 | f3 | f4 ]    next write -> index 1, oldest sample -> index 1
    class FrameStatistics
    {
    public:
        // How many of the last frames are remembered. About 2 seconds at 60 FPS.
        static constexpr std::size_t MaxSampleCount = 120;

        // Adds the duration of a finished frame, in seconds.
        void AddFrame(float frameTime) noexcept;

        // Average duration of the remembered frames in seconds. 0 while no frame was added.
        [[nodiscard]] float GetAverageFrameTime() const noexcept;

        // Frames per second derived from the average frame time. 0 while no frame was added.
        [[nodiscard]] float GetAverageFramesPerSecond() const noexcept;

        // Duration of the longest remembered frame in seconds. 0 while no frame was added.
        [[nodiscard]] float GetLongestFrameTime() const noexcept;

        // The remembered frame times in storage order (not chronological once the buffer has wrapped around).
        // Together with GetOldestSampleIndex() this is exactly what ImGui::PlotLines expects for a ring buffer.
        [[nodiscard]] std::span<const float> GetSamples() const noexcept;

        // Index in GetSamples() of the oldest remembered frame: the graph starts there.
        [[nodiscard]] std::size_t GetOldestSampleIndex() const noexcept;

    private:
        std::array<float, MaxSampleCount> m_samples{};

        // Where the next sample will be written.
        std::size_t m_nextIndex = 0;

        // How many samples are stored: grows up to MaxSampleCount and then stays there.
        std::size_t m_storedSampleCount = 0;
    };
}
