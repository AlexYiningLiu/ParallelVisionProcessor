#include <Utils/PerformanceMetrics.h>
#include <algorithm>
#include <iomanip>
#include <iostream>

void PerformanceMetrics::startTimer(const std::string &name)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mStartTime = std::chrono::high_resolution_clock::now();
}

void PerformanceMetrics::stopTimer(const std::string &name)
{
    std::lock_guard<std::mutex> lock(mMutex);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - mStartTime).count();
    mElapsedTimes[name] = static_cast<double>(duration) / 1000.0; // Convert to seconds
}

double PerformanceMetrics::getElapsedTime(const std::string &name) const
{
    std::lock_guard<std::mutex> lock(mMutex);
    auto it = mElapsedTimes.find(name);
    if (it != mElapsedTimes.end())
    {
        return it->second;
    }
    return 0.0;
}

double PerformanceMetrics::calculateSpeedup(const std::string &baseline, const std::string &comparison) const
{
    double baselineTime = getElapsedTime(baseline);
    double comparisonTime = getElapsedTime(comparison);

    if (comparisonTime > 0)
    {
        return baselineTime / comparisonTime;
    }
    return 0.0;
}

double PerformanceMetrics::calculateEfficiency(const std::string &baseline, const std::string &comparison,
                                               int numThreads) const
{
    double speedup = calculateSpeedup(baseline, comparison);

    if (numThreads > 0)
    {
        return speedup / numThreads;
    }
    return 0.0;
}

void PerformanceMetrics::printMetrics(int numThreads) const
{
    std::cout << "\n=== Performance Metrics ===\n";
    std::cout << std::fixed << std::setprecision(4);

    double singleThreadTime = getElapsedTime("SingleThread");
    double multiThreadTime = getElapsedTime("MultiThread");
    double speedup = calculateSpeedup("SingleThread", "MultiThread");
    double efficiency = calculateEfficiency("SingleThread", "MultiThread", numThreads);

    std::cout << "Single-thread processing time: " << singleThreadTime << " seconds\n";
    std::cout << "Multi-thread processing time: " << multiThreadTime << " seconds\n";
    std::cout << "Number of threads: " << numThreads << "\n";
    std::cout << "Speedup: " << speedup << "x\n";
    std::cout << "Efficiency: " << efficiency << " (speedup per thread)\n";

    // Amdahl's Law analysis
    if (speedup > 0 && numThreads > 1)
    {
        // Estimate the sequential portion using Amdahl's Law
        double sequentialFraction = (1.0 / speedup - 1.0 / numThreads) / (1.0 - 1.0 / numThreads);
        sequentialFraction = std::max(0.0, std::min(1.0, sequentialFraction)); // Clamp to [0,1]

        std::cout << "Estimated sequential portion: " << (sequentialFraction * 100.0) << "%\n";
        std::cout << "Estimated parallel portion: " << ((1.0 - sequentialFraction) * 100.0) << "%\n";

        // Theoretical maximum speedup (Amdahl's Law)
        double theoreticalMaxSpeedup = 1.0 / (sequentialFraction + (1.0 - sequentialFraction) / numThreads);
        std::cout << "Theoretical maximum speedup: " << theoreticalMaxSpeedup << "x\n";
        std::cout << "Achieved " << (speedup / theoreticalMaxSpeedup * 100.0) << "% of theoretical maximum\n";
    }
}
