#pragma once
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

class PerformanceMetrics
{
public:
    PerformanceMetrics() = default;

    // Start timing a named operation
    void startTimer(const std::string &name);

    // Stop timing and record elapsed time
    void stopTimer(const std::string &name);

    // Get elapsed time for a named operation
    double getElapsedTime(const std::string &name) const;

    // Calculate speedup (baseline / comparison)
    double calculateSpeedup(const std::string &baseline, const std::string &comparison) const;

    // Calculate efficiency (speedup / numThreads)
    double calculateEfficiency(const std::string &baseline, const std::string &comparison, int numThreads) const;

    // Print all metrics to console
    void printMetrics(int numThreads) const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
    std::unordered_map<std::string, double> mElapsedTimes;
    mutable std::mutex mMutex;
};
